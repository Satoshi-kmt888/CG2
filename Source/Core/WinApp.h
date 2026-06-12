#pragma once

#include <cstdint>

#include <Windows.h>

/// @brief Windowsアプリケーションの基盤管理を行うシングルトンクラス
class WinApp {
public:
	/*--- 公開定数 ---*/

	static const uint32_t kClientWidth = 1280; //クライアント領域の横幅
	static const uint32_t kClientHeight = 720; //クライアント領域の縦幅

	/*--- インスタンス管理 ---*/

	/// @brief インスタンスの取得
	/// @return 
	static WinApp* GetInstance();

	//コピーガード
	WinApp(const WinApp&) = delete;            //コピーコンストラクタを禁止
	WinApp& operator=(const WinApp&) = delete; //代入演算子を禁止

	/*--- 静的メソッド ---*/

	/// @brief 
	/// @param[in] hwnd 
	/// @param[in] msg 
	/// @param[in] wparam 
	/// @param[in] lparam 
	/// @return 
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// @brief ウィンドウの初期化
	void Initialize();

	/// @brief メッセージの受付処理
	/// @return 
	bool ProcessMessage() const;

	/// @brief ウィンドウの破棄および後処理
	void Finalize();

	/*--- ゲッター ---*/

	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return hInstance_; }

private:
	/*--- コンストラクタ・デストラクタ ---*/

	WinApp() = default;
	~WinApp() = default;

	/*--- 内部変数 ---*/

	HWND hwnd_ = nullptr;           //ウィンドウハンドル
	WNDCLASS wc_ = {};              //ウィンドウクラス構造体
	HINSTANCE hInstance_ = nullptr; //アプリケーションインスタンスハンドル
};
