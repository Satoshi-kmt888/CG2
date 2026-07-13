#pragma once

#include <Windows.h>
#include <cstdint>

/// <summary>
/// ウィンドウの生成 / メッセージ処理 / 破棄を行うクラス
/// </summary>
class WinApp {
public:
	//--- 公開定数 ---

	//デフォルトクライアント領域
	static inline constexpr uint32_t kDefaultClientWidth = 1280;
	static inline constexpr uint32_t kDefaultClientHeight = 720;

	//ウィンドウクラス名
	static inline const wchar_t* kWindowClassName = L"CG2";

	//--- インスタンス管理 ---

	static WinApp* GetInstance();

	//コピーガード
	WinApp(const WinApp&) = delete;
	WinApp& operator=(const WinApp&) = delete;

	//--- 公開関数 ---

	/// <summary>
	/// ウィンドウクラス登録とウィンドウ生成
	/// </summary>
	/// <returns>初期化成功時にtrue</returns>
	bool Initialize();

	/// <summary>
	/// Windowsメッセージ処理
	/// </summary>
	/// <returns>アプリケーションを継続する際はtrue</returns>
	bool ProcessMessage() const;

	/// <summary>
	/// 生成したウィンドウの破棄とクラスの登録解除
	/// </summary>
	void Finalize();

	//--- ゲッター ---

	HINSTANCE GetHInstance() const { return hInstance_; }
	HWND GetHwnd() const { return hWnd_; }

	const uint32_t& GetClientWidth() const { return clientWidth_; }
	const uint32_t& GetClientHeight() const { return clientHeight_; }

private:
	//--- インスタンス管理 ---

	WinApp() = default;
	~WinApp() = default;

	//--- 内部関数 ---

	/// <summary>
	/// Windowsからのイベント・メッセージを処理するコールバック関数
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <param name="msg">メッセージ</param>
	/// <param name="wparam">パラメータ1</param>
	/// <param name="lparam">パラメータ2</param>
	/// <returns></returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//--- 内部変数 ---

	//ウィンドウズ関連
	HINSTANCE hInstance_ = nullptr; //インスタンスハンドル
	HWND hWnd_ = nullptr; //ウィンドウハンドル

	//クライアント領域のサイズ
	uint32_t clientWidth_ = kDefaultClientWidth;
	uint32_t clientHeight_ = kDefaultClientHeight;
};
