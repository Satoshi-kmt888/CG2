#include "App/WinApp.h"

#include "App/StringUtility.h"
#include "Debugger/Logger.h"

WinApp::~WinApp() {
	Destroy();
}

std::unique_ptr<WinApp> WinApp::Create(const wchar_t* title, uint32_t width, uint32_t height) {
	HINSTANCE hinstance = GetModuleHandle(nullptr);
	if (!hinstance) {
		LOG_ERROR("ModuleHandle の取得に失敗しました。");
		return nullptr;
	}

	//1枚目のウィンドウの時のみ登録を行う
	if (s_window_count == 0 && !Register(hinstance, WindowProc)) {
		return nullptr;
	}

	auto instance = std::make_unique<WinApp>();

	RECT rect = {};
	rect.right = static_cast<LONG>(width);
	rect.bottom = static_cast<LONG>(height);

	auto style = WS_OVERLAPPEDWINDOW;
	AdjustWindowRect(&rect, style, FALSE);

	instance->m_hwnd = CreateWindowEx(
	0,                      //拡張するウィンドウのウィンドウスタイル
	k_window_class_name,    //ウィンドウクラス名
	title,                  //タイトルバーに表示する文字列
	style,                  //作成するウィンドウのスタイル
	CW_USEDEFAULT,          //生成されるx座標の初期値(Windowsに任せる)
	CW_USEDEFAULT,          //生成されるy座標の初期値(Windowsに任せる)
	rect.right - rect.left, //ウィンドウの幅
	rect.bottom - rect.top, //ウィンドウの高さ
	nullptr,                //親/オーナーウィンドウハンドルの指定
	nullptr,                //メニュー/子ウィンドウIDの指定
	hinstance,              //インスタンスハンドル
	nullptr                 //オプション
	);

	if (!instance->m_hwnd) {
		LOG_ERROR("ウィンドウの生成に失敗しました。タイトル: {}", StringUtility::ConvertString(title));

		//1枚もウィンドウがない場合に登録が残らないようにする
		if (s_window_count == 0) {
			Unregister(hinstance);
		}

		return nullptr;
	}

	instance->m_hinstance = hinstance;
	instance->m_width = width;
	instance->m_height = height;
	++s_window_count;

	//ウィンドウを表示
	ShowWindow(instance->m_hwnd, SW_SHOWNORMAL);
	UpdateWindow(instance->m_hwnd);
	SetFocus(instance->m_hwnd);

	LOG_INFO("WinApp の初期化が正常に完了しました。クライアント解像度: {}x{}", instance->m_width, instance->m_height);
	return instance;
}

bool WinApp::ProcessMessage() {
	MSG msg{};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT) {
			LOG_INFO("WM_QUIT を検知しました。メッセージループを終了します。");
			return false;
		}
	}

	return true;
}

void WinApp::Destroy() {
	if (!m_hwnd) {
		return;
	}

	if (m_hwnd) {
		DestroyWindow(m_hwnd);
	}
	m_hwnd = nullptr;

	--s_window_count;

	if (s_window_count == 0) {
		Unregister(m_hinstance);
	}
	m_hinstance = nullptr;

	LOG_INFO("ウィンドウの破棄が完了しました。");
}

LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (s_message_hook && s_message_hook(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool WinApp::Register(HINSTANCE hinstance, WNDPROC window_proc) {
	//ウィンドウの設定
	WNDCLASSEX wc = {};
	wc.hInstance = hinstance;
	wc.cbSize = sizeof(WNDCLASSEX);                        //WNDCLASSEX構造体のサイズを指定
	wc.style = CS_HREDRAW | CS_VREDRAW;                    //ウィンドウスタイル(水平・垂直方向の再描画)
	wc.lpfnWndProc = window_proc;                          //ウィンドウプロシージャ
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);         //アプリケーションのアイコン
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);           //アプリケーションで使用するカーソル
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND); //ウィンドウの背景色
	wc.lpszMenuName = nullptr;                             //ウィンドウメニューを表すリソース名(メニューバー)
	wc.lpszClassName = k_window_class_name;                //ウィンドウクラスを識別する名前(アプリケーション名がおすすめ)
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);       //タイトルバーに表示するアイコン(16x16)

	if (!RegisterClassEx(&wc)) {
		LOG_ERROR("ウィンドウクラスの登録に失敗しました。クラス名: {}", StringUtility::ConvertString(k_window_class_name));
		return false;
	}

	return true;
}

void WinApp::Unregister(HINSTANCE hinstance) {
	UnregisterClass(k_window_class_name, hinstance);
}
