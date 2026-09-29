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
	void Initialize() {
		//デバッグ関連の初期化
		CrashHandler::Register();
		Logger::Initialize();

		//ウィンドウの初期化
		auto* winApp = WinApp::GetInstance();
		winApp->Initialize();

		//DirectX12基盤の初期化
		auto* graphicsSystem = GraphicsSystem::GetInstance();
		graphicsSystem->Initialize(winApp->GetHwnd(), winApp->GetClientWidth(), winApp->GetClientHeight());

		//入力処理の初期化
		Input::GetInstance()->Initialize(winApp->GetHInstance(), winApp->GetHwnd());

		//音声系統の初期化
		Audio::GetInstance()->Initialize();

		//マネージャーの初期化
		TextureManager::GetInstance()->Initialize(graphicsSystem->GetDevice(), graphicsSystem->GetCommandList());
		ImGuiManager::GetInstance()->Initialize(
			winApp->GetHwnd(),
			graphicsSystem->GetDevice(),
			graphicsSystem->GetSwapChainDesc().BufferCount,
			graphicsSystem->GetRTVDesc().Format
		);
	}

	void Finalize() {
		//マネージャー類の終了処理
		ImGuiManager::GetInstance()->Finalize();
		TextureManager::GetInstance()->Finalize();

		//音声系統の終了処理
		Audio::GetInstance()->Finalize();

		//DirectX12基盤の終了
		GraphicsSystem::GetInstance()->Finalize();
		//WinAppの終了
		WinApp::GetInstance()->Finalize();
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
		return WinApp::GetInstance()->ProcessMessage();
	}
}//namespace StarEngine

