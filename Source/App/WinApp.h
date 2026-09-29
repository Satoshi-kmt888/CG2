#pragma once

#include <cstdint>
#include <Windows.h>

/// <summary>
/// ウィンドウの生成 / メッセージ処理 / 破棄を行うクラス
/// </summary>
class WinApp {
public:
	//デフォルトクライアント領域
	static inline constexpr uint32_t kDefaultClientWidth = 1280;
	static inline constexpr uint32_t kDefaultClientHeight = 720;

	//ウィンドウクラス名
	static inline const wchar_t* kWindowClassName = L"CG2";

	//==================================================
	// public methods
	//==================================================

	static WinApp* GetInstance();

	//コピームーブ禁止
	WinApp(const WinApp&) = delete;
	WinApp& operator=(const WinApp&) = delete;
	WinApp(const WinApp&&) = delete;
	WinApp& operator=(const WinApp&&) = delete;

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

	HINSTANCE GetHInstance() const { return m_hInstance; }
	HWND GetHwnd() const { return m_hWnd; }

	const uint32_t& GetClientWidth() const { return m_clientWidth; }
	const uint32_t& GetClientHeight() const { return m_clientHeight; }

private:
	//==================================================
	// private methods
	//==================================================

	WinApp() = default;
	~WinApp() = default;

	//==================================================
	// private variables
	//==================================================

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
	HINSTANCE m_hInstance = nullptr; //インスタンスハンドル
	HWND m_hWnd = nullptr; //ウィンドウハンドル

	//クライアント領域のサイズ
	uint32_t m_clientWidth = kDefaultClientWidth;
	uint32_t m_clientHeight = kDefaultClientHeight;
};
