#pragma once

/// <summary>
/// エンジン全体のライフサイクルを管理するコアシステム
/// </summary>
namespace StarEngine {
	/// <summary>
	/// 
	/// </summary>
	void Initialize();

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
