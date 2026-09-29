#pragma once

#include "Graphics/DescriptorManager.h"

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

	//==================================================
	// public methods
	//==================================================

	SwapChain();
	~SwapChain();

	//コピーガード
	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;
	SwapChain(const SwapChain&&) = delete;
	SwapChain& operator=(const SwapChain&&) = delete;

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
	bool Present(uint32_t syncInterval = 1) const;

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

	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return m_swapChainDesc; }
	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return m_rtvDesc; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle() const {
		UINT index = m_swapChain->GetCurrentBackBufferIndex();
		return m_rtvHandles[index].cpuHandle;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() const { return m_dsvHandle.cpuHandle; }

private:
	//==================================================
	// private methods
	//==================================================

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

	//==================================================
	// private variables
	//==================================================

	//表示関連
	ComPtr<IDXGISwapChain4> m_swapChain;
	DXGI_SWAP_CHAIN_DESC1 m_swapChainDesc{};
	std::array<ComPtr<ID3D12Resource>, kBufferCount> m_swapChainResources;

	//RTV(Render Target View)関連
	D3D12_RENDER_TARGET_VIEW_DESC m_rtvDesc{};
	std::array<DescriptorHandle, kBufferCount> m_rtvHandles{};

	//DSV(Depth Stencil View)関連
	D3D12_DEPTH_STENCIL_VIEW_DESC m_dsvDesc{};
	DescriptorHandle m_dsvHandle{};
	ComPtr<ID3D12Resource> m_depthStencilResource;
};

