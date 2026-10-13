#pragma once

#include <cstdint>

/// <summary>
/// エンジン全体のライフサイクルを管理するコアシステム
/// </summary>
namespace StarEngine {
	/// <summary>
	/// 
	/// </summary>
	void Initialize(const wchar_t* window_title, uint32_t window_width = 1280, uint32_t window_height = 720);

	/// <summary>
	/// 
	/// </summary>
	void Finalize();

	/// <summary>
	/// 
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// 
	/// </summary>
	void EndFrame();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool ProcessMessage();
}//namespace StarEngine
