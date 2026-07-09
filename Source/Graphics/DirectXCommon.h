#pragma once

#include <cstdint>
#include <d3d12.h>
#include <dxgi1_2.h>
#include <dxgi1_5.h>
#include <dxgi1_6.h>
#include <Windows.h>
#include <wrl/client.h>

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
public:
	//--- インスタンス管理 ---

	/**
	 * \brief インスタンスの取得
	 * \return DirectXCommon唯一のインスタンス
	 */
	static DirectXCommon* GetInstance();

	//コピーガード
	DirectXCommon(const DirectXCommon&) = delete;
	DirectXCommon& operator=(const DirectXCommon&) = delete;

	//--- 公開関数 ---

	/** \brief 初期化処理 */
	void Initialize();

	/** \brief フレーム描画開始処理 */
	void PreDraw();

	/** \brief フレーム描画終了処理 */
	void PostDraw();

	/** \brief 終了処理*/
	void Finalize();

	//--- ゲッター ---

	ID3D12Device* GetDevice() const { return device_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc_; }
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() const { return rtvDesc_; }
	ID3D12DescriptorHeap* GetDsvDescriptorHeap() const { return dsvDescriptorHeap_.Get(); }

private:
	//--- コンストラクタ・デストラクタ ---

	DirectXCommon() = default;
	~DirectXCommon() = default;

	//--- 内部関数 ---

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

	//--- メンバ変数 ---

	//基盤
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	//コマンド関連
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	//表示関連
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };

	//RTV(Render Target View)関連
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2]{};
	uint32_t rtvDescriptorSize_ = 0;

	//SRV(Shader Resource View)関連
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_{};
	uint32_t dsvDescriptorSize_ = 0;

	//同期・その他
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_ = nullptr;
	uint32_t backBufferIndex_ = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
};
