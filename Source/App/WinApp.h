#pragma once

#include <cstdint>
#include <memory>
#include <Windows.h>

/// <summary>
/// ウィンドウの生成 / メッセージ処理 / 破棄を行うクラス
/// </summary>
class WinApp {
public:
	using MessageHook = bool (*)(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//==================================================
	// public methods
	//==================================================

	WinApp() = default;
	~WinApp();

	//コピー・ムーブ禁止
	WinApp(const WinApp&) = delete;
	WinApp& operator=(const WinApp&) = delete;
	WinApp(WinApp&&) = delete;
	WinApp& operator=(WinApp&&) = delete;

	static std::unique_ptr<WinApp> Create(const wchar_t* window_title, uint32_t width = 1280, uint32_t height = 720);

	/// <summary>
	/// Windowsメッセージ処理
	/// </summary>
	/// <returns>アプリケーションを継続する際はtrue</returns>
	static bool ProcessMessage();

	/// <summary>
	/// 生成したウィンドウの破棄とクラスの登録解除
	/// </summary>
	void Destroy();

	static void SetMessageHook(MessageHook message_hook) { s_message_hook = message_hook; }

	HINSTANCE GetHInstance() const { return m_hinstance; }
	HWND GetHwnd() const { return m_hwnd; }

	const uint32_t& GetClientWidth() const { return m_width; }
	const uint32_t& GetClientHeight() const { return m_height; }

private:
	//==================================================
	// private methods
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

	static bool Register(HINSTANCE hinstance, WNDPROC window_proc);
	static void Unregister(HINSTANCE hinstance);

	//==================================================
	// private variables
	//==================================================

	static constexpr const wchar_t* k_window_class_name = L"Engine";

	static inline uint32_t s_window_count = 0;
	static inline MessageHook s_message_hook = nullptr;

	//ウィンドウズ関連
	HINSTANCE m_hinstance = nullptr;
	HWND m_hwnd = nullptr;

	//クライアント領域のサイズ
	uint32_t m_width = 0;
	uint32_t m_height = 0;
};
