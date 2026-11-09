#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

/// <summary>
/// GPUデバイス管理クラス
/// </summary>
class GraphicsDevice {
public:
	//==================================================
	// public methods
	//==================================================

	GraphicsDevice() = default;
	~GraphicsDevice();

	//コピー・ムーブ禁止
	GraphicsDevice(const GraphicsDevice&) = delete;
	GraphicsDevice& operator=(const GraphicsDevice&) = delete;
	GraphicsDevice(const GraphicsDevice&&) = delete;
	GraphicsDevice& operator=(const GraphicsDevice&&) = delete;

	/// <summary>
	/// DXGIFactory/Adapter/Deviceの初期化およびデバッグ設定
	/// </summary>
	/// <returns>初期化成功時にtrue</returns>
	bool Initialize();

	/// <summary>
	/// DXGIFactory/Adapter/Deviceの解放
	/// </summary>
	void Finalize();

	IDXGIFactory7* GetDxgiFactory() const { return m_dxgiFactory.Get(); }
	ID3D12Device* GetDevice() const { return m_device.Get(); }

private:
	//==================================================
	// private methods
	//==================================================

#ifdef _DEBUG
	/// <summary>
	/// デバッグレイヤーの有効化
	/// </summary>
	void EnableDebugLayer() const;
#endif

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

#ifdef _DEBUG
	/// <summary>
	/// デバッグ時のエラー・警告検知フィルター(InfoQueue)の設定
	/// </summary>
	void ConfigureInfoQueue() const;
#endif

	//==================================================
	// private variables
	//==================================================

	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	bool m_initialized = false;

	ComPtr<IDXGIFactory7> m_dxgiFactory;
	ComPtr<IDXGIAdapter4> m_adapter;
	ComPtr<ID3D12Device> m_device;
};
