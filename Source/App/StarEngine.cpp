#include "App/StarEngine.h"

#include "App/WinApp.h"
#include "Audio/Audio.h"
#include "Debugger/CrashHandler.h"
#include "Debugger/ImGuiManager.h"
#include "Debugger/Logger.h"
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/ShaderCompiler.h"
#include "Input/Input.h"
#include "Scene/DirectionalLight.h"
#include "Scene/TextureManager.h"

#include <memory>

#include <d3d12.h>

namespace StarEngine {
	void Initialize() {
		CrashHandler::Register();
		Logger::Initialize();

		//基盤システムの初期化
		auto* winApp = WinApp::GetInstance();
		winApp->Initialize();

		auto* graphicsSystem = GraphicsSystem::GetInstance();
		graphicsSystem->Initialize(*winApp);

		auto device = GraphicsSystem::GetInstance()->GetDevice();
		auto commandList = GraphicsSystem::GetInstance()->GetCommandList();

		//マネージャー(とりあえずテクスチャのみ)
		TextureManager::GetInstance()->Initialize(device, commandList);
		Audio::GetInstance()->Initialize();
		Input::GetInstance()->Initialize();
		ImGuiManager::GetInstance()->Initialize(
			winApp->GetHwnd(), graphicsSystem->GetDevice(),
			graphicsSystem->GetSwapChainDesc().BufferCount,
			graphicsSystem->GetRTVDesc().Format
		);
	}

	void Finalize() {
		ImGuiManager::GetInstance()->Finalize();
		Audio::GetInstance()->Finalize();
		TextureManager::GetInstance()->Finalize();

		GraphicsSystem::GetInstance()->Finalize();
		WinApp::GetInstance()->Finalize();

		//ログファイルの終了
		Logger::Finalize();
	}

	void BeginFrame() {
		Input::GetInstance()->Update();

		GraphicsSystem::GetInstance()->PreDraw();
		//IMGUI
		ImGuiManager::GetInstance()->BeginFrame();

		//SRV用のヒープ
		ID3D12DescriptorHeap* descriptorHeap[] = { TextureManager::GetInstance()->GetSrvDescriptorHeap() };
		GraphicsSystem::GetInstance()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);
	}

	void EndFrame() {
		auto commandList = GraphicsSystem::GetInstance()->GetCommandList();
		ImGuiManager::GetInstance()->EndFrame(commandList);

		//描画後処理
		GraphicsSystem::GetInstance()->PostDraw();
	}

	bool ProcessMessage() {
		return WinApp::GetInstance()->ProcessMessage();
	}
}//namespace StarEngine

