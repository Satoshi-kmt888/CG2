#pragma once

#include <Windows.h>
#include <d3d12.h>

/// <summary>
/// ImGuiの 初期化 / フレーム管理 / 描画 / 破棄 を行うシングルトンクラス
/// </summary>
class ImGuiManager {
public:
	//--- インスタンス管理 ---

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static ImGuiManager* GetInstance();

	//--- コピーガード ---

	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;

	//--- 公開関数 ---

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="device"></param>
	/// <param name="bufferCount"></param>
	/// <param name="rtvFormat"></param>
	void Initialize(HWND hwnd, ID3D12Device* device, int bufferCount, DXGI_FORMAT rtvFormat);

	/// <summary>
	/// 
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="commandList"></param>
	void EndFrame(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 
	/// </summary>
	void Finalize();

private:
	//--- インスタンス管理 ---

	ImGuiManager() = default;
	~ImGuiManager() = default;
};

