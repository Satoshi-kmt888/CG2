#include "MyEngine.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif

#include <Windows.h>
#include <sal.h>

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	MyEngine::Initialize();

	//ウィンドウの×ボタンが押されるまでループ
	while (MyEngine::ProcessMessage()) {
		MyEngine::BeginFrame();

		MyEngine::EndFrame();
	}

	MyEngine::Finalize();

	return 0;
}
