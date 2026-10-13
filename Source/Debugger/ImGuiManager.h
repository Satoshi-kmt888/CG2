#pragma once

#include "Graphics/DescriptorManager.h"

#include <d3d12.h>
#include <dxgiformat.h>
#include <Windows.h>

/// <summary>
/// ImGuiの 初期化 / フレーム管理 / 描画 / 破棄 を行うシングルトンクラス
/// </summary>
class ImGuiManager {
public:
	//==================================================
	// publice methods
	//==================================================

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static ImGuiManager* GetInstance();

	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="device"></param>
	/// <param name="bufferCount"></param>
	/// <param name="rtvFormat"></param>
	void Initialize(HWND hwnd, ID3D12Device* device, int bufferCount, DXGI_FORMAT rtvFormat);

	/// <summary>
	/// フレーム開始処理
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// フレーム終了処理
	/// </summary>
	/// <param name="commandList"></param>
	void EndFrame(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	static bool HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//==================================================
	// private methods
	//==================================================

	ImGuiManager() = default;
	~ImGuiManager() = default;

	//==================================================
	// private variables
	//==================================================

	DescriptorHandle descriptorHandle_{};
	HWND m_hwnd = nullptr;
};

