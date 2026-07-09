#pragma once

#include "Graphics/GraphicsDevice.h"
#include "Graphics/CommandContext.h"
#include "Graphics/SwapChain.h"

#include <array>

class WinApp;

/// <summary>
/// DirectX12の各コンポーネントを統括し、描画パイプラインの前後処理を管理するシングルトンクラス
/// </summary>
class GraphicsSystem {
public:
	//--- インスタンス管理 ---

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static GraphicsSystem* GetInstance();

	//コピーガード
	GraphicsSystem(const GraphicsSystem&) = delete;
	GraphicsSystem& operator=(const GraphicsSystem&) = delete;

	//--- 公開関数 ---

	/// <summary>
	/// グラフィックスシステムを構成するすべてのコンポーネントを初期化する
	/// </summary>
	/// <param name="winApp">接続先のウィンドウアプリケーションの参照</param>
	/// <returns>初期化成功時にtrue</returns>
	bool Initialize(const WinApp& winApp);

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

	//--- ゲッター ---

	ID3D12Device* GetDevice() const { return device_.GetDevice(); }
	ID3D12GraphicsCommandList* GetCommandList() { return command_.GetCommandList(); }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChain_.GetSwapChainDesc(); }
	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return swapChain_.GetRTVDesc(); }

private:
	//--- インスタンス管理 ---

	GraphicsSystem() = default;
	~GraphicsSystem() = default;

	//--- 内部変数 ---

	//画面のクリアカラー
	static constexpr std::array<float, 4> kClearColor = { 0.392f, 0.584f, 0.929f, 1.0f };

	GraphicsDevice device_;  //GPUデバイス管理
	CommandContext command_; //コマンドキュー・リスト/同期の管理
	SwapChain swapChain_;    //バックバッファ表示・深度バッファの管理
};
