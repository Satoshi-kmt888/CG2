#include "WinApp.h"

#include "Logger.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

#include <cassert>
#include <format>

#ifdef _DEBUG
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
#endif

	//メッセージに応じてゲーム固有の処理を行う
	//ウィンドウが破棄された
	if (msg == WM_DESTROY) {
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::Initialize() {
	//COMの初期化
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(hr == S_OK || hr == S_FALSE);

	hInstance_ = GetModuleHandle(nullptr);

	wc_.style = CS_HREDRAW | CS_VREDRAW; //サイズ変更時に再描画を要求
	wc_.lpfnWndProc = WindowProc;
	wc_.lpszClassName = L"CG2";
	wc_.hInstance = hInstance_;
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc_.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)); //ホワイトアウト対策：背景を黒ブラシで塗りつぶす

	//ウィンドウクラスを登録する
	RegisterClass(&wc_);

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0, 0, kClientWidth, kClientHeight };
	//クライアント領域をもとに実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウの生成
	hwnd_ = CreateWindow(
		wc_.lpszClassName,    //利用するクラス名
		L"CG2",               //タイトルバーの文字
		WS_OVERLAPPEDWINDOW,  //ウィンドウスタイル
		CW_USEDEFAULT,        //表示X座標(Windowsに任せる)
		CW_USEDEFAULT,        //表示Y座標(Windowsに任せる)
		wrc.right - wrc.left, //ウィンドウ横幅
		wrc.bottom - wrc.top, //ウィンドウ縦幅
		nullptr,              //親ウィンドウハンドル
		nullptr,              //メニューハンドル
		hInstance_,           //インスタンスハンドル
		nullptr               //オプション
	);
	assert(hwnd_ != nullptr);

	//ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);

	Debug::Log(std::format("WinApp Initialize Succeeded. ClientSize: {}x{}\n", kClientWidth, kClientHeight));
}

bool WinApp::ProcessMessage() const {
	MSG msg{};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT) {
			return false;
		}
	}

	return true;
}

void WinApp::Finalize() {
	if (hwnd_) {
		DestroyWindow(hwnd_);
		hwnd_ = nullptr;
	}

	//ウィンドウクラスの登録解除
	UnregisterClass(wc_.lpszClassName, hInstance_);

	//COMを終了
	CoUninitialize();
}
