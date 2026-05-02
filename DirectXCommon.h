#pragma once
#include <cstdint>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <dxgi1_2.h>
#include <dxgi1_5.h>
#include <Windows.h>

/**
 * \class DirectXCommon
 * \brief DirectX12の基盤を管理するシングルトンクラス
 * * 役割:
 * - GPUデバイスの生成と管理
 * - コマンドリスト、キュー、アロケータの制御
 * - スワップチェーンによる画面表示の管理
 * - 各種ディスクリプターヒープの保持
 */
class DirectXCommon {
public://--- インスタンス制御 ---
	/**
	 * \brief インスタンスの取得
	 * \return DirectXCommon唯一のインスタンス
	 */
	static DirectXCommon* GetInstance();

	//コピーガード
	DirectXCommon(const DirectXCommon&) = delete;
	DirectXCommon& operator=(const DirectXCommon&) = delete;

public://--- ライフサイクル ---
	/** \brief 初期化処理 */
	void Initialize();

	/** \brief フレーム描画開始処理 */
	void PreDraw();

	/** \brief フレーム描画終了処理 */
	void PostDraw();

	/** \brief  終了処理*/
	void Finalize();

public://--- ゲッター ---
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc; }
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() const { return rtvDesc; }
	ID3D12DescriptorHeap* GetDsvDescriptorHeap() const { return dsvDescriptorHeap.Get(); }

private://--- 内部初期化関数 ---
	/** \brief デバッグレイヤーの有効化 */
	void EnableDebugLayer();

	/** \brief DXGIファクトリとアダプタの作成、デバイスの生成 */
	void CreateDevice();

	/** \brief コマンドキュー、アロケータ、コマンドリストの生成 */
	void CreateCommand();

	/** \brief スワップチェーンの作成 */
	void CreateSwapChain();

	/** \brief RTV/SRV用ディスクリプターヒープとレンダーターゲットの作成 */
	void CreateFinalRenderTargets();

	/** \brief GPUとの同期用フェンスの作成 */
	void CreateFence();

private://--- コンストラクタ・デストラクタ
	DirectXCommon() = default;
	~DirectXCommon() = default;

private://--- メンバ変数 ---
	//基盤
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;

	//コマンド関連
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

	//表示関連
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };

	//ディスクリプターヒープ関連
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};

	//同期・その他
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent = nullptr;
	uint32_t backBufferIndex = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;
};
