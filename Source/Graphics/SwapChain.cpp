#include "SwapChain.h"

#include "D3D12Utility.h"
#include "Debugger/Logger.h"

#include <dxgi.h>
#include <dxgiformat.h>

using namespace D3D12Utility;

SwapChain::~SwapChain() {
	Finalize();
}

bool SwapChain::Initialize(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue, ID3D12Device* device,
	HWND hwnd, uint32_t width, uint32_t height) {
	if (!dxgiFactory || !commandQueue || !device || !hwnd) {
		LOG_ERROR("スワップチェーンの初期化に失敗。引数にnullptrが含まれています。");
		return false;
	}

	if (!CreateSwapChain(dxgiFactory, commandQueue, hwnd, width, height)) {
		Finalize();
		return false;
	}

	if (!CreateRenderTargetViews(device)) {
		Finalize();
		return false;
	}

	if (!CreateDepthStencilView(device, width, height)) {
		Finalize();
		return false;
	}

	LOG_INFO("スワップチェーンの初期化が正常に完了しました。");
	return true;
}

bool SwapChain::Present(uint32_t syncInterval) {
	HRESULT hr = swapChain_->Present(syncInterval, 0);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
		LOG_ERROR("Present中にデバイスロストを検知しました。HRESULT: 0x{0:X}", static_cast<uint32_t>(hr));
		return false;
	}

	return true;
}

void SwapChain::TransitionToRenderTarget(ID3D12GraphicsCommandList* commandList) {
	UINT index = swapChain_->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResources_[index].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;       // 表示状態から
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画可能状態へ
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier);
}

void SwapChain::TransitionToPresent(ID3D12GraphicsCommandList* commandList) {
	UINT index = swapChain_->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResources_[index].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画可能状態から
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;       // 表示状態へ
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier);
}

void SwapChain::Finalize() {
	depthStencilResource_.Reset();
	for (auto& resource : swapChainResources_) {
		resource.Reset();
	}
	dsvDescriptorHeap_.Reset();
	rtvDescriptorHeap_.Reset();
	swapChain_.Reset();
}

bool SwapChain::CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue,
	HWND hwnd, uint32_t width, uint32_t height) {
	//スワップチェーンを生成する
	swapChainDesc_.Width = width;                                 //画面の幅
	swapChainDesc_.Height = height;                               //画面の高さ
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;           //色の形式
	swapChainDesc_.SampleDesc.Count = 1;                          //マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //描画のターゲットとして利用する
	swapChainDesc_.BufferCount = kBufferCount;                    //バッファ枚数
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    //モニタにうつしたら中身を破棄
	ComPtr<IDXGISwapChain1> swapChain1;

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue,
		hwnd,
		&swapChainDesc_,
		nullptr,
		nullptr,
		&swapChain1
	);
	if (FAILED(hr)) {
		LOG_ERROR("HWND用のスワップチェーン生成に失敗しました。");
		return false;
	}

	hr = swapChain1.As(&swapChain_);
	if (FAILED(hr)) {
		LOG_ERROR("IDXGISwapChain4への型キャストに失敗しました。");
		return false;
	}

	return true;
}

bool SwapChain::CreateRenderTargetViews(ID3D12Device* device) {
	//DSVディスクリプターヒープの生成とサイズ取得
	rtvDescriptorHeap_ = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	if (!rtvDescriptorHeap_) {
		LOG_ERROR("RTV用ディスクリプターヒープの生成に失敗しました。");
		return false;
	}
	rtvDescriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//RTVの設定
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	   //出力結果をSRGBに変換して書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; //2dテクスチャとして書き込む

	//ディスクリプタハンドルの割り当て
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	for (uint32_t i = 0; i < kBufferCount; ++i) {
		HRESULT hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
		if (FAILED(hr)) {
			LOG_ERROR("スワップチェーンからのバッファ取得に失敗しました。インデックス: [{}]", i);
			return false;
		}

		rtvHandles_[i] = rtvHandle;
		device->CreateRenderTargetView(swapChainResources_[i].Get(), &rtvDesc_, rtvHandle);

		rtvHandle.ptr += rtvDescriptorSize_;
	}

	return true;
}

bool SwapChain::CreateDepthStencilView(ID3D12Device* device, uint32_t width, uint32_t height) {
	//DSVディスクリプターヒープの生成とサイズ取得
	dsvDescriptorHeap_ = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	if (!dsvDescriptorHeap_) {
		LOG_ERROR("DSV用ディスクリプターヒープの生成に失敗しました。");
		return false;
	}
	dsvDescriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//深度バッファ用テクスチャリソースの生成
	depthStencilResource_ = CreateDepthStencilTextureResource(device, width, height);
	if (!depthStencilResource_) {
		LOG_ERROR("深度ステンシル用テクスチャリソースの生成に失敗しました。");
		return false;
	}

	//DSVの設定
	dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //Format。基本的にはResourceに合わせる
	dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; //2dTexture

	//DSVHeapの先頭にDSVを作る
	dsvHandle_ = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	device->CreateDepthStencilView(
		depthStencilResource_.Get(),
		&dsvDesc_,
		dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart()
	);

	return true;
}
