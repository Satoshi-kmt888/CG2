#include "DirectXCommon.h"
#include "WinApp.h"

#include "Camera.h"
#include "DebugUtil.h"
#include "GraphicsPipeline.h"
#include "Model.h"
#include "Object3D.h"
#include "ShaderCompiler.h"
#include "TextureManager.h"
#include "Vector3.h"

#ifdef USE_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>
#endif

#include <memory>

#include <Windows.h>
#include <d3d12.h>
#include <dxgidebug.h>
#include <strsafe.h>
#include <wrl/client.h>
#include <dxgi1_3.h>
#include <d3d12sdklayers.h>
#include <sal.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//誰も捕捉しなかった場合に(Unhandled)、捕捉する関数を登録
	SetUnhandledExceptionFilter(ExportDump);
	{
		//==================================================
		// 初期化
		//==================================================

		//ログファイル
		InitializeLog();

		//ウィンドウズアプリケーション
		WinApp::GetInstance()->Initialize();

		//DirectX12の基盤
		DirectXCommon::GetInstance()->Initialize();

		//シェーダーコンパイラを生成・初期化
		std::unique_ptr<ShaderCompiler> shaderCompiler = std::make_unique<ShaderCompiler>();
		shaderCompiler->Initialize();

		//グラフィックスパイプラインを生成・初期化
		std::unique_ptr<GraphicsPipeline> graphicsPipeline = std::make_unique<GraphicsPipeline>();
		graphicsPipeline->Initialize(shaderCompiler.get());

		//テクスチャマネージャーを生成・初期化
		TextureManager::GetInstance()->Initialize(
			DirectXCommon::GetInstance()->GetDevice(),
			DirectXCommon::GetInstance()->GetCommandList()
		);

		//3Dカメラを生成・初期化
		std::unique_ptr<Camera> camera3D = std::make_unique<Camera>();
		camera3D->Initialize(
			WinApp::GetInstance()->kClientWidth,
			WinApp::GetInstance()->kClientHeight
		);

		//2Dカメラを生成・初期化
		std::unique_ptr<Camera> camera2D = std::make_unique<Camera>();
		camera2D->SetProjectionType(ProjectionType::Orthographic);
		camera2D->Initialize(
			WinApp::GetInstance()->kClientWidth,
			WinApp::GetInstance()->kClientHeight
		);

		//スプライトのモデル
		std::unique_ptr<Model> spriteModel = Model::CreateQuad();
		spriteModel->SetTexture("resources/uvChecker.png");

		//球のモデル
		std::unique_ptr<Model> sphereModel = Model::CreateSphere();
		sphereModel->SetTexture("resources/uvChecker.png");
		bool useMonsterBall = true; //<! モンスターボールのテクスチャ判別フラグ
		bool enableLighting = true; //<! ライトの有効化

		//スプライトの生成・初期化
		Object3D sprite{};
		sprite.Initialize();
		sprite.SetModel(spriteModel.get());

		//球の生成・初期化
		Object3D sphere{};
		sphere.Initialize();
		sphere.SetModel(sphereModel.get());

		//IMGUI
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

		//ウィンドウの×ボタンが押されるまでループ
		while (WinApp::GetInstance()->ProcessMessage() != 0) {
			//==================================================
			// 更新
			//==================================================

			//IMGUI
#ifdef USE_IMGUI
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			/*--- カメラ ---*/
			//座標
			ImGui::DragFloat3("CameraTranslation", &camera3D->GetTranslation().x, 0.01f);
			ImGui::DragFloat3("CameraRotation", &camera3D->GetRotation().x, 0.01f);

			//スプライト
			ImGui::DragFloat3("sprite translate", &sprite.GetTranslation().x, 1.0f); //<! 座標

			/*--- 球 ---*/
			//色の変更
			ImGui::ColorEdit4("materialColor", &sphere.GetMaterialColor().x); //<! RGBA

			//ライティングの切り替え
			ImGui::Checkbox("enableLighting", &enableLighting);
			if (enableLighting) {
				sphere.SetMaterialEnableLighting(true);
			} else {
				sphere.SetMaterialEnableLighting(false);
			}

			//テクスチャの切り替え
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			if (useMonsterBall) {
				sphereModel->SetTexture("resources/monsterball.png");
				sphere.SetModel(sphereModel.get());
			} else {
				sphereModel->SetTexture("resources/uvChecker.png");
				sphere.SetModel(sphereModel.get());
			}

			/*--- ライト ---*/
			//色の変更
			ImGui::ColorEdit4("material", &sphere.GetDirectionalLightColor().x); //<! RGBA

			//向き
			ImGui::SliderFloat3("LightDirection", &sphere.GetDirectionalLightDirection().x, -1.0f, 1.0f, "%.3f");

			//輝度
			ImGui::DragFloat("Intensity", &sphere.GetDirectionalLightIntensity(), 0.001f, 0.0f, 1.0f, "%.3f");
#endif

			//カメラの更新
			camera2D->Update();
			camera3D->Update();

			//スプライト
			//sprite.Update(camera2D->GetViewProjMatrix());

			//球
			sphere.Update(camera3D->GetViewProjMatrix());
			sphere.SetRotation(sphere.GetRotation() + Vector3(0.0f, 0.01f, 0.0f));

			//==================================================
			// 描画
			//==================================================

			//描画前処理
			DirectXCommon::GetInstance()->PreDraw();

#ifdef USE_IMGUI
			//ImGUiの描画コマンドを確定させる
			ImGui::Render();
#endif

			//SRV用のヒープ
			ID3D12DescriptorHeap* descriptorHeap[] = { TextureManager::GetInstance()->GetSrvDescriptorHeap() };
			DirectXCommon::GetInstance()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);

			DirectXCommon::GetInstance()->GetCommandList()->RSSetViewports(1, &camera3D->GetViewport());
			DirectXCommon::GetInstance()->GetCommandList()->RSSetScissorRects(1, &camera3D->GetScissorRect());
			//RootSignatureを設定。PSOとは別途設定が必要
			DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(graphicsPipeline->GetRootSignature());
			DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(graphicsPipeline->GetGraphicsPipelineState());

			//スプライト
			//sprite.Draw();

			//球
			sphere.Draw();

#ifdef USE_IMGUI
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXCommon::GetInstance()->GetCommandList());
#endif

			//描画後処理
			DirectXCommon::GetInstance()->PostDraw();
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
		//テクスチャマネージャーを終了
		TextureManager::GetInstance()->Finalize();

		//DirectX12基盤を終了
		DirectXCommon::GetInstance()->Finalize();

		//ウィンドウズアプリケーションを終了
		WinApp::GetInstance()->Finalize();
	}

	//ログファイルの終了
	FinalizeLog();

	//リソースリークチェック
	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}

	return 0;
}
