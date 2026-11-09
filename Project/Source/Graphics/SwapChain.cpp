#include "SwapChain.h"

#include "Debugger/Logger.h"
#include "Graphics/D3D12Utility.h"

#include <dxgi.h>
#include <dxgiformat.h>

SwapChain::SwapChain() = default;

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

bool SwapChain::Present(uint32_t syncInterval) const {
	if (HRESULT hr = m_swapChain->Present(syncInterval, 0);
		hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
		LOG_ERROR("Present中にデバイスロストを検知しました。HRESULT: 0x{0:X}", static_cast<uint32_t>(hr));
		return false;
	}

	return true;
}

void SwapChain::TransitionToRenderTarget(ID3D12GraphicsCommandList* commandList) {
	UINT index = m_swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_swapChainResources[index].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;       // 表示状態から
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画可能状態へ
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier);
}

void SwapChain::TransitionToPresent(ID3D12GraphicsCommandList* commandList) {
	UINT index = m_swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_swapChainResources[index].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画可能状態から
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;       // 表示状態へ
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier);
}

void SwapChain::Finalize() {
	for (uint32_t i = 0; i < kBufferCount; ++i) {
		DescriptorManager::GetInstance()->Free(m_rtvHandles[i]);
	}

	//DSVハンドルの返却
	DescriptorManager::GetInstance()->Free(m_dsvHandle);

	for (auto& resource : m_swapChainResources) {
		resource.Reset();
	}
	m_depthStencilResource.Reset();
	m_swapChain.Reset();
}

bool SwapChain::CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue,
	HWND hwnd, uint32_t width, uint32_t height) {
	//スワップチェーンを生成する
	m_swapChainDesc.Width = width;                                 //画面の幅
	m_swapChainDesc.Height = height;                               //画面の高さ
	m_swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;           //色の形式
	m_swapChainDesc.SampleDesc.Count = 1;                          //マルチサンプルしない
	m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //描画のターゲットとして利用する
	m_swapChainDesc.BufferCount = kBufferCount;                    //バッファ枚数
	m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    //モニタにうつしたら中身を破棄
	ComPtr<IDXGISwapChain1> swapChain1;

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue,
		hwnd,
		&m_swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1
	);
	if (FAILED(hr)) {
		LOG_ERROR("HWND用のスワップチェーン生成に失敗しました。");
		return false;
	}

	hr = swapChain1.As(&m_swapChain);
	if (FAILED(hr)) {
		LOG_ERROR("IDXGISwapChain4への型キャストに失敗しました。");
		return false;
	}

	return true;
}

bool SwapChain::CreateRenderTargetViews(ID3D12Device* device) {
	m_rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	m_rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (uint32_t i = 0; i < kBufferCount; ++i) {
		//スワップチェーンからリソース（バックバッファ）を取得
		HRESULT hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainResources[i]));
		if (FAILED(hr)) {
			LOG_ERROR("スワップチェーンからのバッファ取得に失敗しました。インデックス: [{}]", i);
			return false;
		}

		//統括マネージャからRTV用のハンドルを1つずつ「割り当て」
		m_rtvHandles[i] = DescriptorManager::GetInstance()->Allocate(DescriptorType::RTV);

		//もらったハンドルの「cpuHandle」を使ってRTVを生成
		device->CreateRenderTargetView(m_swapChainResources[i].Get(), &m_rtvDesc, m_rtvHandles[i].cpuHandle);
	}

	return true;
}

bool SwapChain::CreateDepthStencilView(ID3D12Device* device, uint32_t width, uint32_t height) {
	m_depthStencilResource = D3D12Utility::CreateDepthStencilTextureResource(device, width, height);
	if (!m_depthStencilResource) {
		LOG_ERROR("深度ステンシル用テクスチャリソースの生成に失敗しました。");
		return false;
	}

	m_dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;         // リソースとフォーマットを合わせる
	m_dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	m_dsvHandle = DescriptorManager::GetInstance()->Allocate(DescriptorType::DSV);

	device->CreateDepthStencilView(
		m_depthStencilResource.Get(),
		&m_dsvDesc,
		m_dsvHandle.cpuHandle
	);

	return true;
}
