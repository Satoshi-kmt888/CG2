#include "StarEngine.h"

#include "Camera.h"
#include "DebugUtil.h"
#include "DirectXCommon.h"
#include "GraphicsPipeline.h"
#include "ShaderCompiler.h"
#include "TextureManager.h"
#include "DirectionalLight.h"
#include "WinApp.h"

#ifdef USE_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>
#endif

#include <memory>

#include <Windows.h>
#include <dxgidebug.h>
#include <dxgi1_3.h>
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <wrl/client.h>

namespace StarEngine {
	//--- 内部静的変数 ---
	static std::unique_ptr<ShaderCompiler> shaderCompiler = nullptr;
	static std::unique_ptr<GraphicsPipeline> graphicsPipeline = nullptr;
	static std::unique_ptr<Camera> camera = nullptr;
	static std::unique_ptr<DirectionalLight> directionalLight = nullptr;

	void Initialize() {
		SetUnhandledExceptionFilter(ExportDump);
		//ログファイル
		InitializeLog();

		//基盤システムの初期化
		WinApp::GetInstance()->Initialize();
		DirectXCommon::GetInstance()->Initialize();

		//コンパイラとパイプラインの生成・初期化
		shaderCompiler = std::make_unique<ShaderCompiler>();
		shaderCompiler->Initialize();
		graphicsPipeline = std::make_unique<GraphicsPipeline>();
		graphicsPipeline->Initialize(shaderCompiler.get());

		auto device = DirectXCommon::GetInstance()->GetDevice();
		auto commandList = DirectXCommon::GetInstance()->GetCommandList();

		//マネージャー(とりあえずテクスチャのみ)
		TextureManager::GetInstance()->Initialize(device, commandList);

		//カメラ
		camera = std::make_unique<Camera>();
		camera->Initialize(WinApp::GetInstance()->kClientWidth, WinApp::GetInstance()->kClientHeight);

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

		graphicsPipeline.reset();
		shaderCompiler.reset();
		camera.reset();
		directionalLight.reset();

		//テクスチャマネージャーを終了
		TextureManager::GetInstance()->Finalize();

		//DirectX12基盤を終了
		DirectXCommon::GetInstance()->Finalize();

		//ウィンドウズアプリケーションを終了
		WinApp::GetInstance()->Finalize();

		//ログファイルの終了
		FinalizeLog();

		//リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}

	void BeginFrame(){
		//IMGUI
#ifdef USE_IMGUI
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
#endif
		DirectXCommon::GetInstance()->PreDraw();

		//SRV用のヒープ
		ID3D12DescriptorHeap* descriptorHeap[] = { TextureManager::GetInstance()->GetSrvDescriptorHeap() };
		DirectXCommon::GetInstance()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);

		auto commandList = DirectXCommon::GetInstance()->GetCommandList();
		commandList->RSSetViewports(1, &camera->GetViewport());
		commandList->RSSetScissorRects(1, &camera->GetScissorRect());
		//RootSignatureを設定。PSOとは別途設定が必要
		commandList->SetGraphicsRootSignature(graphicsPipeline->GetRootSignature());
		commandList->SetPipelineState(graphicsPipeline->GetGraphicsPipelineState());
		commandList->SetGraphicsRootConstantBufferView(3, directionalLight->GetGPUVirtualAddress());
	}

	void EndFrame(){
#ifdef USE_IMGUI
		//ImGUiの描画コマンドを確定させる
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXCommon::GetInstance()->GetCommandList());
#endif

		//描画後処理
		DirectXCommon::GetInstance()->PostDraw();
	}

	bool ProcessMessage(){
		return WinApp::GetInstance()->ProcessMessage();
	}
}//namespace StarEngine
