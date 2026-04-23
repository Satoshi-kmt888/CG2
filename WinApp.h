#pragma once

#include <Windows.h>
#include <cstdint>

/// <summary>
/// ウィンドウズアプリケーション
/// </summary>
class WinApp {
public:
	WinApp() = default;
	~WinApp();

	//ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void Initialize();
	bool ProcessMessage();
	void Finalize();

	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return hInstance_; }

	//クライアント領域のサイズ
	static const uint32_t kClientWidth = 1280;
	static const uint32_t kClientHeight = 720;

private:
	HWND hwnd_ = nullptr;
	WNDCLASS wc_ = {};
	HINSTANCE hInstance_ = nullptr;
};
