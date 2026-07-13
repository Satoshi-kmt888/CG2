#include "App/WinApp.h"

#include "App/StringUtility.h"
#include "Debugger/Logger.h"

#include <imgui.h>

#ifdef _DEBUG
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

bool WinApp::Initialize() {
	//アプリケーションのインスタンスハンドルを取得
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	if (!hInstance) {
		//インスタンスハンドルの取得失敗ログ
		LOG_ERROR("ModuleHandle の取得に失敗しました。");
		return false;
	}

	//ウィンドウの設定
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);                        //WNDCLASSEX構造体のサイズを指定
	wc.style = CS_HREDRAW | CS_VREDRAW;                    //ウィンドウスタイル(水平・垂直方向の再描画)
	wc.lpfnWndProc = WindowProc;                           //ウィンドウプロシージャ
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);       //アプリケーションのアイコン
	wc.hCursor = LoadCursor(hInstance, IDC_ARROW);         //アプリケーションで使用するカーソル
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND); //ウィンドウの背景色
	wc.lpszMenuName = nullptr;                             //ウィンドウメニューを表すリソース名(メニューバー)
	wc.lpszClassName = kWindowClassName;                   //ウィンドウクラスを識別する名前(アプリケーション名がおすすめ)
	wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);     //タイトルバーに表示するアイコン(16x16)

	//ウィンドウクラスの登録
	if (!RegisterClassEx(&wc)) {
		//ウィンドウクラスの登録失敗ログ
		LOG_ERROR("ウィンドウクラスの登録に失敗しました。クラス名: {}", StringUtility::ConvertString(kWindowClassName));
		return false;
	}

	//インスタンスハンドルの設定
	hInstance_ = hInstance;

	//ウィンドウサイズを設定
	RECT rect = {};
	rect.right = static_cast<LONG>(clientWidth_);
	rect.bottom = static_cast<LONG>(clientHeight_);

	//ウィンドウサイズを調整
	auto style = WS_OVERLAPPEDWINDOW;
	AdjustWindowRect(&rect, style, FALSE);

	//ウィンドウを生成
	hWnd_ = CreateWindowEx(
	0,                                  //拡張するウィンドウのウィンドウスタイル
	kWindowClassName,                   //ウィンドウクラス名
	kWindowClassName,                   //タイトルバーに表示する文字列
	style,                              //作成するウィンドウのスタイル
	CW_USEDEFAULT,                      //生成されるx座標の初期値(Windowsに任せる)
	CW_USEDEFAULT,                      //生成されるy座標の初期値(Windowsに任せる)
	rect.right - rect.left,             //ウィンドウの幅
	rect.bottom - rect.top,             //ウィンドウの高さ
	nullptr,                            //親/オーナーウィンドウハンドルの指定
	nullptr,                            //メニュー/子ウィンドウIDの指定
	hInstance_,                         //インスタンスハンドル
	this                                //オプション
	);

	if (!hWnd_) {
		//ウィンドウ生成の失敗ログ
		LOG_ERROR("ウィンドウの生成に失敗しました。");
		return false;
	}

	//ウィンドウを表示
	ShowWindow(hWnd_, SW_SHOWNORMAL);
	UpdateWindow(hWnd_);
	SetFocus(hWnd_);

	LOG_INFO("WinApp の初期化が正常に完了しました。クライアント解像度: {}x{}", clientWidth_, clientHeight_);

	return true;
}

bool WinApp::ProcessMessage() const {
	MSG msg{};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		//ループ中にWM_QUITを検出したら即座に終了
		if (msg.message == WM_QUIT) {
			//終了メッセージ検知ログ
			LOG_INFO("WM_QUIT を検知しました。メッセージループを終了します。");
			return false;
		}
	}

	//正常終了
	return true;
}

void WinApp::Finalize() {
	if (hWnd_) {
		DestroyWindow(hWnd_);
		hWnd_ = nullptr;
	}

	if (hInstance_) {
		//ウィンドウの登録解除
		UnregisterClass(kWindowClassName, hInstance_);
		hInstance_ = nullptr;
	}

	LOG_INFO("WinApp の解放処理が完了しました。");
}

LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
#endif

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}
