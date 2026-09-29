#pragma once

#include "Graphics/CommandContext.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"

#include <array>

class WinApp;

/// <summary>
/// DirectX12の各コンポーネントを統括し、描画パイプラインの前後処理を管理するシングルトンクラス
/// </summary>
class GraphicsSystem {
public:
	//==================================================
	// public methods
	//==================================================

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static GraphicsSystem* GetInstance();

	//コピームーブ禁止
	GraphicsSystem(const GraphicsSystem&) = delete;
	GraphicsSystem& operator=(const GraphicsSystem&) = delete;
	GraphicsSystem(const GraphicsSystem&&) = delete;
	GraphicsSystem& operator=(const GraphicsSystem&&) = delete;

	/// <summary>
	/// グラフィックスシステムを構成するすべてのコンポーネントを初期化する
	/// </summary>
	/// <param name="winApp">接続先のウィンドウアプリケーションの参照</param>
	/// <returns>初期化成功時にtrue</returns>
	bool Initialize(HWND hwnd, uint32_t width, uint32_t height);

	/// <summary>
	/// 描画コマンドの記録を開始し、レンダーターゲットをクリアする
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画コマンドの記録を終了して実行し、画面をフリップする
	/// </summary>
	void PostDraw();

	/// <summary>
	/// グラフィックスシステム全体の解放処理を行う
	/// </summary>
	void Finalize();

	ID3D12Device* GetDevice() const { return m_device.GetDevice(); }
	ID3D12GraphicsCommandList* GetCommandList() { return m_command.GetCommandList(); }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return m_swapChain.GetSwapChainDesc(); }
	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return m_swapChain.GetRTVDesc(); }

private:
	//==================================================
	// private methods
	//==================================================

	GraphicsSystem() = default;
	~GraphicsSystem() = default;

	//==================================================
	// private variables
	//==================================================

	//画面のクリアカラー
	static constexpr std::array<float, 4> kClearColor = { 0.392f, 0.584f, 0.929f, 1.0f };

	GraphicsDevice m_device;  //GPUデバイス管理
	CommandContext m_command; //コマンドキュー・リスト/同期の管理
	SwapChain m_swapChain;    //バックバッファ表示・深度バッファの管理

	D3D12_RECT m_scissorRect{};
	D3D12_VIEWPORT m_viewport{};
};
