#include "StarEngine.h"

#include "Audio/AudioManager.h"
#include "Core/WinApp.h"
#include "Diagnostics/CrashHandler.h"
#include "Diagnostics/Logger.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/ShaderCompiler.h"
#include "Graphics/TextureManager.h"
#include "Input/InputManager.h"
#include "Light/DirectionalLight.h"
#include "Diagnostics/ImGuiManager.h"

#include <memory>

#include <d3d12.h>

namespace StarEngine {
	//--- 内部静的変数 ---

	static std::unique_ptr<ShaderCompiler> shaderCompiler = nullptr;
	static std::unique_ptr<GraphicsPipeline> graphicsPipeline = nullptr;
	static std::unique_ptr<DirectionalLight> directionalLight = nullptr;

	void Initialize() {
		CrashHandler::Register();
		Logger::Initialize();

		//基盤システムの初期化
		auto* winApp = WinApp::GetInstance();
		winApp->Initialize();

		auto* graphicsSystem = GraphicsSystem::GetInstance();
		graphicsSystem->Initialize(*winApp);

		//コンパイラとパイプラインの生成・初期化
		shaderCompiler = std::make_unique<ShaderCompiler>();
		shaderCompiler->Initialize();
		graphicsPipeline = std::make_unique<GraphicsPipeline>();
		graphicsPipeline->Initialize(shaderCompiler.get());

		auto device = GraphicsSystem::GetInstance()->GetDevice();
		auto commandList = GraphicsSystem::GetInstance()->GetCommandList();

		//マネージャー(とりあえずテクスチャのみ)
		TextureManager::GetInstance()->Initialize(device, commandList);
		AudioManager::GetInstance()->Initialize();
		InputManager::GetInstance()->Initialize();
		ImGuiManager::GetInstance()->Initialize(
			winApp->GetHwnd(), graphicsSystem->GetDevice(),
			graphicsSystem->GetSwapChainDesc().BufferCount,
			graphicsSystem->GetRTVDesc().Format
		);

		//ライト
		directionalLight = std::make_unique<DirectionalLight>();
		directionalLight->Initialize();
	}

	void Finalize() {
		ImGuiManager::GetInstance()->Finalize();

		//static変数を明示的にリセット(寿命の問題があるので必ず!)
		shaderCompiler.reset();
		graphicsPipeline.reset();
		directionalLight.reset();

		////基盤類を終了させる
		AudioManager::GetInstance()->Finalize();
		TextureManager::GetInstance()->Finalize();
		GraphicsSystem::GetInstance()->Finalize();
		WinApp::GetInstance()->Finalize();

		//ログファイルの終了
		Logger::Finalize();
	}

	void BeginFrame() {
		GraphicsSystem::GetInstance()->PreDraw();
		//IMGUI
		ImGuiManager::GetInstance()->BeginFrame();
		InputManager::GetInstance()->Update();


		//SRV用のヒープ
		ID3D12DescriptorHeap* descriptorHeap[] = { TextureManager::GetInstance()->GetSrvDescriptorHeap() };
		GraphicsSystem::GetInstance()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);

		directionalLight->Update();

		auto commandList = GraphicsSystem::GetInstance()->GetCommandList();
		//RootSignatureを設定。PSOとは別途設定が必要
		commandList->SetGraphicsRootSignature(graphicsPipeline->GetRootSignature());
		commandList->SetPipelineState(graphicsPipeline->GetGraphicsPipelineState());
		commandList->SetGraphicsRootConstantBufferView(3, directionalLight->GetGPUVirtualAddress());
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

