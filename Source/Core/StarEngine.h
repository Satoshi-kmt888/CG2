#pragma once

/**
 * \namespace StarEngine
 * \brief エンジン全体のライフサイクルを管理するコアシステム
 */
namespace StarEngine {
	/**
	 * \brief 初期化処理
	 */
	void Initialize();

	/**
	 * \brief 終了処理
	 */
	void Finalize();

	/**
	 * \brief フレーム開始処理
	 */
	void BeginFrame();

	/**
	 * \brief フレーム終了処理
	 */
	void EndFrame();

	/**
	 * \brief Windowsメッセージの処理
	 */
	bool ProcessMessage();
}//namespace StarEngine
