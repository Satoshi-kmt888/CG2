#include "StarEngine.h"

#include "Audio/AudioManager.h"
#include "Logging/Logger.h"
#include "Logging/CrashHandler.h"
#include "Light/DirectionalLight.h"
#include "Graphics/DirectXCommon.h"
#include "Graphics/GraphicsPipeline.h"
#include "Input/InputManager.h"
#include "Graphics/ShaderCompiler.h"
#include "Graphics/TextureManager.h"
#include "FrameWork/WinApp.h"

#ifdef USE_IMGUI
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>
#endif

#include <memory>

#include <d3d12.h>

#include <Windows.h>

namespace StarEngine {
	//--- 内部静的変数 ---

	static std::unique_ptr<ShaderCompiler> shaderCompiler = nullptr;
	static std::unique_ptr<GraphicsPipeline> graphicsPipeline = nullptr;
	static std::unique_ptr<DirectionalLight> directionalLight = nullptr;

	//シザー矩形の設定
	static D3D12_RECT scissorRect{};

	//クライアント領域のサイズと一緒にして画面全体に表示
	static D3D12_VIEWPORT viewport{};

	void Initialize() {
		CrashHandler::Register();

		//ログファイル
		Logger::Initialize();

		//基盤システムの初期化
		WinApp::GetInstance()->Initialize();
		DirectXCommon::GetInstance()->Initialize();

		LONG width = static_cast<LONG>(WinApp::GetInstance()->kClientWidth);
		LONG height = static_cast<LONG>(WinApp::GetInstance()->kClientHeight);

		scissorRect = D3D12_RECT{ 0, 0, width, height };
		viewport = D3D12_VIEWPORT{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };

		//コンパイラとパイプラインの生成・初期化
		shaderCompiler = std::make_unique<ShaderCompiler>();
		shaderCompiler->Initialize();
		graphicsPipeline = std::make_unique<GraphicsPipeline>();
		graphicsPipeline->Initialize(shaderCompiler.get());

		auto device = DirectXCommon::GetInstance()->GetDevice();
		auto commandList = DirectXCommon::GetInstance()->GetCommandList();

		//マネージャー(とりあえずテクスチャのみ)
		TextureManager::GetInstance()->Initialize(device, commandList);
		AudioManager::GetInstance()->Initialize();
		InputManager::GetInstance()->Initialize();

		//ライト
		directionalLight = std::make_unique<DirectionalLight>();
		directionalLight->Initialize();

#ifdef USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHwnd());
		ImGui_ImplDX12_Init(
			DirectXCommon::GetInstance()->GetDevice(),
			DirectXCommon::GetInstance()->GetSwapChainDesc().BufferCount,
			DirectXCommon::GetInstance()->GetRtvDesc().Format,
			TextureManager::GetInstance()->GetSrvDescriptorHeap(),
			TextureManager::GetInstance()->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			TextureManager::GetInstance()->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
		);
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Build();
#endif
	}

	void Finalize() {
#ifdef USE_IMGUI
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

		//static変数を明示的にリセット(寿命の問題があるので必ず!)
		graphicsPipeline.reset();
		shaderCompiler.reset();
		directionalLight.reset();

		//基盤類を終了させる
		AudioManager::GetInstance()->Finalize();
		TextureManager::GetInstance()->Finalize();
		DirectXCommon::GetInstance()->Finalize();
		WinApp::GetInstance()->Finalize();

		//ログファイルの終了
		Logger::Finalize();
	}

	void BeginFrame() {
		//IMGUI
#ifdef USE_IMGUI
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
#endif
		InputManager::GetInstance()->Update();

		DirectXCommon::GetInstance()->PreDraw();

		//SRV用のヒープ
		ID3D12DescriptorHeap* descriptorHeap[] = { TextureManager::GetInstance()->GetSrvDescriptorHeap() };
		DirectXCommon::GetInstance()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);

		directionalLight->Update();

		auto commandList = DirectXCommon::GetInstance()->GetCommandList();
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissorRect);
		//RootSignatureを設定。PSOとは別途設定が必要
		commandList->SetGraphicsRootSignature(graphicsPipeline->GetRootSignature());
		commandList->SetPipelineState(graphicsPipeline->GetGraphicsPipelineState());
		commandList->SetGraphicsRootConstantBufferView(3, directionalLight->GetGPUVirtualAddress());
	}

	void EndFrame() {
#ifdef USE_IMGUI
		//ImGUiの描画コマンドを確定させる
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXCommon::GetInstance()->GetCommandList());
#endif

		//描画後処理
		DirectXCommon::GetInstance()->PostDraw();
	}

	bool ProcessMessage() {
		return WinApp::GetInstance()->ProcessMessage();
	}
}//namespace StarEngine

