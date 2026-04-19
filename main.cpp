#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <format>
#include <strsafe.h>
#include <iostream>

#include "WinApp.h"
#include "DirectXCommon.h"
#include "ShaderCompiler.h"
#include "GraphicsPipeline.h"
#include "Object3D.h"
#include "DebugUtil.h"
#include "Matrix4x4.h"
#include "Transform.h"

#ifdef USE_IMGUI
#include "imgui.h"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"
#endif

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//誰も捕捉しなかった場合に(Unhandled)、捕捉する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

	//==================================================
	//                     初期化
	//==================================================

	//ログファイル
	InitLog();

	//ウィンドウズアプリケーションを生成・初期化
	WinApp* winApp = new WinApp();
	winApp->Initialize();
	Log(std::cout,
		std::format(
			"WinApp Initialize Succeeded. ClientSize: {}x{}\n",
			winApp->kClientWidth, winApp->kClientHeight
		));

	//DirectX12の基盤を生成・初期化
	DirectXCommon* dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);
	Log(std::format(
		"DirectXCommon Initialize Succeeded.\n"
	));

	//シェーダーコンパイラを生成・初期化
	ShaderCompiler* shaderCompiler = new ShaderCompiler();
	shaderCompiler->Initialize();
	Log(std::format(
		"ShaderCompiler Initialize Succeeded.\n"
	));

	//グラフィックスパイプラインを生成・初期化
	GraphicsPipeline* graphicsPipeline = new GraphicsPipeline();
	graphicsPipeline->Initialize(dxCommon->GetDevice(), shaderCompiler);
	Log(std::format(
		"GraphicsPipeline Initialize Succeeded.\n"
	));

	//オブジェクト(三角形)を生成・初期化
	Object3D* triangle = new Object3D();
	triangle->Initialize(dxCommon->GetDevice());

	//IMGUI
#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(
		dxCommon->GetDevice(),
		dxCommon->GetSwapChainDesc().BufferCount,
		dxCommon->GetRtvDesc().Format,
		dxCommon->GetSrvDescriptorHeap(),
		dxCommon->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxCommon->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
	);
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Build();
#endif

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = winApp->kClientWidth;
	viewport.Height = winApp->kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = winApp->kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = winApp->kClientHeight;

	//ウィンドウの×ボタンが押されるまでループ
	while (winApp->ProcessMessage() != 0) {
		//==================================================
		//                       更新
		//==================================================

		//IMGUI
#ifdef USE_IMGUI
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//デモウィンドウの表示
		ImGui::ShowDemoWindow();
#endif

		//カメラのワールド変換データ
		Transform cameraTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -5.0f} };
		Matrix4x4 cameraMatrix =
			Matrix4x4::MakeAffineMatrix(
				cameraTransform.scale, cameraTransform.rotation, cameraTransform.translation
			);
		Matrix4x4 viewMatrix = cameraMatrix.Inversed();
		Matrix4x4 projectionMatrix =
			Matrix4x4::MakeProjectionFovMatrix(
				0.45f, static_cast<float>(winApp->kClientWidth) / static_cast<float>(winApp->kClientHeight), 0.1f, 100.0f
			);
		Matrix4x4 viewProjectionMatrix = viewMatrix * projectionMatrix;

		//三角形の更新処理
		triangle->Update(viewProjectionMatrix);

		//==================================================
		//                       描画
		//==================================================

		//描画前処理
		dxCommon->PreDraw();

#ifdef USE_IMGUI
		//
		ImGui::Render();
#endif

		//SRV用のヒープ
		ID3D12DescriptorHeap* descriptorHeap[] = { dxCommon->GetSrvDescriptorHeap() };
		dxCommon->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);

		dxCommon->GetCommandList()->RSSetViewports(1, &viewport);
		dxCommon->GetCommandList()->RSSetScissorRects(1, &scissorRect);
		//RootSignatureを設定。PSOとは別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(graphicsPipeline->GetRootSignature());
		dxCommon->GetCommandList()->SetPipelineState(graphicsPipeline->GetGraphicsPipelineState());

		//三角形の描画処理
		triangle->Draw(dxCommon->GetCommandList());

#ifdef USE_IMGUI
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
#endif

		//描画後処理
		dxCommon->PostDraw();
	}

	//==================================================
	//                    解放作業
	//==================================================

	//ImGui
#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

	//
	triangle->Finalize();
	delete triangle;

	//グラフィックスパイプラインの開放
	graphicsPipeline->Finalize();
	delete graphicsPipeline;

	//シェーダーコンパイラの開放
	shaderCompiler->Finalize();
	delete shaderCompiler;

	//DirectX12関連の開放
	dxCommon->Finalize();
	delete dxCommon;

	//ウィンドウズアプリケーションの開放
	winApp->Finalize();
	delete winApp;

	//リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

	return 0;
}
