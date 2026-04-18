#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <format>
#include <strsafe.h>

#include "WinApp.h"
#include "DirectXCommon.h"
#include "ShaderCompiler.h"
#include "GraphicsPipeline.h"
#include "D3D12Util.h"
#include "DebugUtil.h"
#include "Vector4.h"
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

	//ウィンドウズアプリケーションを生成・初期化
	WinApp* winApp = new WinApp();
	winApp->Initialize();
	Log(std::format(
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

	//==================================================
	//ImGui
	//==================================================

	//------------------------------
	//初期化
	//------------------------------

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

	//==================================================
	//頂点データの作成とビュー
	//==================================================

	/*
	Resourceの生成
	------------------------------*/
	//頂点リソース
	ID3D12Resource* vertexResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(Vector4) * 3);

	//マテリアルリソース。color1つ分のサイズを用意する
	ID3D12Resource* materialResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(Vector4));
	//マテリアルのデータを書き込む
	Vector4* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//赤を書き込む
	*materialData = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	//WVP用のリソースを作る
	ID3D12Resource* wvpResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(Matrix4x4));
	//データを書き込む
	Matrix4x4* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	*wvpData = Matrix4x4::Identity();

	/*
	VertexBufferViewの作成
	------------------------------*/
	//頂点バッファービューを作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//リソースの先頭アドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(Vector4) * 3;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(Vector4);

	/*
	Resourceにデータを書き込む
	------------------------------*/
	//頂点リソースにデータを書き込む
	Vector4* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//左下
	vertexData[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	//上
	vertexData[1] = { 0.0f, 0.5f, 0.0f, 1.0f };
	//右下
	vertexData[2] = { 0.5f, -0.5f, 0.0f, 1.0f };

	/*
	ViewportとScissor
	------------------------------*/
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

	//Transform変数を作る
	Transform transform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };


	//ウィンドウの×ボタンが押されるまでループ
	while (winApp->ProcessMessage() != 0) {
		//ゲームの処理
#ifdef USE_IMGUI
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//デモウィンドウの表示
		ImGui::ShowDemoWindow();
#endif

		//==================================================
		//三角形の更新
		//==================================================

		//回転
		transform.rotation.y += 0.01f;

		//カメラのワールド変換データ
		Transform cameraTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -5.0f} };

		//ワールド行列更新
		Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);
		Matrix4x4 cameraMatrix =
			Matrix4x4::MakeAffineMatrix(
				cameraTransform.scale, cameraTransform.rotation, cameraTransform.translation
			);
		Matrix4x4 viewMatrix = cameraMatrix.Inversed();
		Matrix4x4 projectionMatrix =
			Matrix4x4::MakeProjectionFovMatrix(
				0.45f, static_cast<float>(winApp->kClientWidth) / static_cast<float>(winApp->kClientHeight), 0.1f, 100.0f
			);
		Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
		*wvpData = worldViewProjectionMatrix;


		//描画前処理
		dxCommon->PreDraw();

#ifdef USE_IMGUI
		//
		ImGui::Render();
#endif

		//
		ID3D12DescriptorHeap* descriptorHeap[] = { dxCommon->GetSrvDescriptorHeap() };
		dxCommon->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);

		//==================================================
		//三角形の描画
		//==================================================

		/*
		コマンドを積む
		------------------------------*/
		dxCommon->GetCommandList()->RSSetViewports(1, &viewport);
		dxCommon->GetCommandList()->RSSetScissorRects(1, &scissorRect);
		//RootSignatureを設定。PSOとは別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(graphicsPipeline->GetRootSignature());
		dxCommon->GetCommandList()->SetPipelineState(graphicsPipeline->GetGraphicsPipelineState());
		dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
		//形状を設定。PSOとは別途設定。同じものを設定
		dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//マテリアルCBufferの場所を設定(RootParameter配列の0番目)
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
		//描画(DrawCall)
		dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);

#ifdef USE_IMGUI
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
#endif

		//描画処理
		dxCommon->PostDraw();
	}

	//==================================================
	//解放作業
	//==================================================

	//三角形の描画に利用したもの
	wvpResource->Release();
	materialResource->Release();
	vertexResource->Release();

#ifdef USE_IMGUI
	//ImGui
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

	//PSOの開放
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
