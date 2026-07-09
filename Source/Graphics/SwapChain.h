#pragma once

#include <array>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <Windows.h>
#include <wrl/client.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// スワップチェーンと描画バッファ(RTV / DSV)を管理するクラス
/// </summary>
class SwapChain {
public:
	//--- 公開定数 ---

	//バッファ枚数
	static constexpr uint32_t kBufferCount = 2;

	//--- インスタンス管理 ---

	SwapChain() = default;
	~SwapChain();

	//コピーガード
	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;

	//---公開関数 ---

	/// <summary>
	/// スワップチェーンおよび描画先レンダーターゲットの初期化
	/// </summary>
	/// <param name="dxgiFactory">DXGIファクトリオブジェクトへのポインタ</param>
	/// <param name="commandQueue">コマンドキューオブジェクトへのポインタ</param>
	/// <param name="device">DirectX12デバイスオブジェクトへのポインタ</param>
	/// <param name="hwnd">接続するウィンドウのハンドル</param>
	/// <param name="width">クライアント領域の横幅</param>
	/// <param name="height">クライアント領域の縦幅</param>
	/// <returns>初期化成功時にtrue</returns>
	bool Initialize(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue, ID3D12Device* device,
		HWND hwnd, uint32_t width, uint32_t height);

	/// <summary>
	/// バックバッファのフリップ
	/// </summary>
	/// <param name="syncInterval">垂直同期のインターバル</param>
	/// <returns>デバイスロストが起きていなければtrue</returns>
	bool Present(uint32_t syncInterval = 1);

	/// <summary>
	/// 現在のバックバッファの状態を「表示用」から「レンダーターゲット用」に遷移させる
	/// </summary>
	/// <param name="commandList">コマンドを記録するグラフィックスコマンドリストへのポインタ</param>
	void TransitionToRenderTarget(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 現在のバックバッファの状態を「レンダーターゲット用」から「表示用」に遷移させる
	/// </summary>
	/// <param name="commandList">コマンドを記録するグラフィックスコマンドリストへのポインタ</param>
	void TransitionToPresent(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// すべてのリソースを解放する
	/// </summary>
	void Finalize();

	//--- ゲッター ---

	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc_; }
	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return rtvDesc_; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle() const {
		UINT index = swapChain_->GetCurrentBackBufferIndex();
		return rtvHandles_[index];
	}
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() const { return dsvHandle_; }

private:
	//--- 内部関数 ---

	/// <summary>
	/// スワップチェーン本体を生成する内部関数
	/// </summary>
	/// <param name="dxgiFactory">DXGIファクトリオブジェクトへのポインタ</param>
	/// <param name="commandQueue">コマンドキューオブジェクトへのポインタ</param>
	/// <param name="hwnd">接続するウィンドウのハンドル</param>
	/// <param name="width">クライアント領域の横幅</param>
	/// <param name="height">クライアント領域の縦幅</param>
	/// <returns>生成成功時にtrue</returns>
	bool CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue,
		HWND hwnd, uint32_t width, uint32_t height);

	/// <summary>
	/// レンダーターゲットビュー(RTV)を生成する
	/// </summary>
	/// <param name="device"></param>
	/// <returns>生成成功時にtrue</returns>
	bool CreateRenderTargetViews(ID3D12Device* device);

	/// <summary>
	/// 深度ステンシルビュー(DSV)を生成する
	/// </summary>
	/// <param name="device">DirectX12デバイスオブジェクトへのポインタ</param>
	/// <param name="width">テクスチャの幅</param>
	/// <param name="height">テクスチャの高さ</param>
	/// <returns>生成成功時にtrue</returns>
	bool CreateDepthStencilView(ID3D12Device* device, uint32_t width, uint32_t height);

	//--- 内部変数 ---

	//表示関連
	ComPtr<IDXGISwapChain4> swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	std::array<ComPtr<ID3D12Resource>, kBufferCount> swapChainResources_;

	//RTV(Render Target View)関連
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kBufferCount> rtvHandles_{};
	uint32_t rtvDescriptorSize_ = 0;

	//DSV(Depth Stencil View)関連
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_{};
	uint32_t dsvDescriptorSize_ = 0;
	ComPtr<ID3D12Resource> depthStencilResource_;
};

