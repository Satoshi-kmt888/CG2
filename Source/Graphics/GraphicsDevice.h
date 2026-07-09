#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// GPUデバイス管理クラス
/// </summary>
class GraphicsDevice {
public:
	//--- インスタンス管理 ---

	GraphicsDevice() = default;
	~GraphicsDevice();

	//コピーガード
	GraphicsDevice(const GraphicsDevice&) = delete;
	GraphicsDevice& operator=(const GraphicsDevice&) = delete;

	//--- 公開関数 ---

	/// <summary>
	/// DXGIFactory/Adapter/Deviceの初期化およびデバッグ設定
	/// </summary>
	/// <returns>初期化成功時にtrue</returns>
	bool Initialize();

	/// <summary>
	/// DXGIFactory/Adapter/Deviceの解放
	/// </summary>
	void Finalize();

	//--- ゲッター ---

	IDXGIFactory7* GetDxgiFactory() const { return dxgiFactory_.Get(); }
	ID3D12Device* GetDevice() const { return device_.Get(); }

private:
	//--- 内部関数 ---

	/// <summary>
	/// デバッグレイヤーの有効化
	/// </summary>
	void EnableDebugLayer() const;

	/// <summary>
	/// DXGIファクトリの生成
	/// </summary>
	/// <returns>生成成功時にtrue</returns>
	bool CreateDxgiFactory();

	/// <summary>
	/// 高パフォーマンスな物理GPU(アダプター)の選定
	/// </summary>
	/// <returns>適切なアダプターが見つかった場合にtrue</returns>
	bool SelectAdapter();

	/// <summary>
	/// 選択されたアダプターを基にD3D12デバイスを生成
	/// </summary>
	/// <returns>生成成功時にtrue</returns>
	bool CreateDevice();

	/// <summary>
	/// デバッグ時のエラー・警告検知フィルター(InfoQueue)の設定
	/// </summary>
	void ConfigureInfoQueue() const;

	//--- 内部変数 ---

	ComPtr<IDXGIFactory7> dxgiFactory_;
	ComPtr<IDXGIAdapter4> useAdapter_;
	ComPtr<ID3D12Device> device_;
};
