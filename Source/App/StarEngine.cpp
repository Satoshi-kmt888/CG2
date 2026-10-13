#include "App/StarEngine.h"

#include "App/WinApp.h"
#include "Audio/Audio.h"
#include "Debugger/CrashHandler.h"
#include "Debugger/ImGuiManager.h"
#include "Debugger/Logger.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/TextureManager.h"
#include "Input/Input.h"

namespace StarEngine {
	namespace {
		std::unique_ptr<WinApp> s_main_window;
	}

	void Initialize(const wchar_t* window_title, uint32_t window_width, uint32_t window_height) {
		//デバッグ関連の初期化
		CrashHandler::Register();
		Logger::Initialize();

		//ウィンドウの初期化
		s_main_window = WinApp::Create(window_title, window_width, window_height);
		if (!s_main_window) {
			LOG_ERROR("メインウィンドウの生成に失敗しました。");
			return;
		}

		//DirectX12基盤の初期化
		auto* graphicsSystem = GraphicsSystem::GetInstance();
		graphicsSystem->Initialize(s_main_window->GetHwnd(), s_main_window->GetClientWidth(), s_main_window->GetClientHeight());

		//入力処理の初期化
		if (!Input::GetInstance()->Initialize(s_main_window->GetHInstance(), s_main_window->GetHwnd())) {
			return;
		}

		//音声系統の初期化
		Audio::GetInstance()->Initialize();

		//マネージャーの初期化
		TextureManager::GetInstance()->Initialize(graphicsSystem->GetDevice(), graphicsSystem->GetCommandList());
		ImGuiManager::GetInstance()->Initialize(
			s_main_window->GetHwnd(),
			graphicsSystem->GetDevice(),
			graphicsSystem->GetSwapChainDesc().BufferCount,
			graphicsSystem->GetRTVDesc().Format
		);

		WinApp::SetMessageHook(ImGuiManager::HandleWindowMessage);
	}

	void Finalize() {
		WinApp::SetMessageHook(nullptr);

		//マネージャー類の終了処理
		ImGuiManager::GetInstance()->Finalize();
		TextureManager::GetInstance()->Finalize();

		//音声系統の終了処理
		Audio::GetInstance()->Finalize();

		//DirectX12基盤の終了
		GraphicsSystem::GetInstance()->Finalize();
		//WinAppの終了
		s_main_window->Destroy();
		//ログクラスの終了
		Logger::Finalize();
	}

	void BeginFrame() {
		Input::GetInstance()->Update();

		GraphicsSystem::GetInstance()->PreDraw();
		ImGuiManager::GetInstance()->BeginFrame();
	}

	void EndFrame() {
		ImGuiManager::GetInstance()->EndFrame(GraphicsSystem::GetInstance()->GetCommandList());
		GraphicsSystem::GetInstance()->PostDraw();
	}

	bool ProcessMessage() {
		return WinApp::ProcessMessage();
	}
}//namespace StarEngine

