#include "App/StarEngine.h"

#include "App/WinApp.h"
#include "Debugger/CrashHandler.h"
#include "Debugger/Logger.h"
#include "Graphics/GraphicsSystem.h"

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
		graphicsSystem->Initialize(*winApp);
	}

	void Finalize() {
		//DirectX12基盤の終了
		GraphicsSystem::GetInstance()->Finalize();
		//WinAppの終了
		WinApp::GetInstance()->Finalize();
		//ログクラスの終了
		Logger::Finalize();
	}

	void BeginFrame() {
		GraphicsSystem::GetInstance()->PreDraw();
	}

	void EndFrame() {
		GraphicsSystem::GetInstance()->PostDraw();
	}

	bool ProcessMessage() {
		return WinApp::GetInstance()->ProcessMessage();
	}
}//namespace StarEngine

