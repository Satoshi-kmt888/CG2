#include "DirectXCommon.h"
#include "WinApp.h"

#include "D3D12Util.h"
#include "DebugUtil.h"
#include "GraphicsPipeline.h"
#include "Object3D.h"
#include "Sprite.h"
#include "Sphere.h"
#include "ShaderCompiler.h"
#include "TextureLoader.h"
#include "Transform.h"
#include "Camera.h"

#include <externals/DirectXTex/DirectXTex.h>
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
		//                     初期化
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

		//オブジェクト(三角形)を生成・初期化
		std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
		sphere->Initialize(DirectXCommon::GetInstance()->GetSrvDescriptorHeap());

		//スプライトを生成・初期化
		std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize(DirectXCommon::GetInstance()->GetSrvDescriptorHeap());

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
			DirectXCommon::GetInstance()->GetSrvDescriptorHeap(),
			DirectXCommon::GetInstance()->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			DirectXCommon::GetInstance()->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
		);
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Build();
#endif


		//Textureを読んで転送する
		DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(DirectXCommon::GetInstance()->GetDevice(), metadata);
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource.Get(), mipImages);

		//metadataをもとにSRVを設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
		//SRVの生成
		DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(
			textureResource.Get(),
			&srvDesc,
			sphere->GetTextureSrvHandleCPU()
		);


		//ウィンドウの×ボタンが押されるまでループ
		while (WinApp::GetInstance()->ProcessMessage() != 0) {
			//==================================================
			//                       更新
			//==================================================

			//IMGUI
#ifdef USE_IMGUI
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			//スプライトの座標変更
			ImGui::DragFloat3("translateSprite", &sprite->GetTranslate().x, 1.0f);
#endif

			//カメラの更新
			camera3D->Update();

			//三角形の更新処理
			sphere->Update(camera3D->GetViewProjMatrix());

			//スプライトの更新処理
			sprite->Update(camera2D->GetViewProjMatrix());

			//==================================================
			//                       描画
			//==================================================

			//描画前処理
			DirectXCommon::GetInstance()->PreDraw();

#ifdef USE_IMGUI
			//ImGUiの描画コマンドを確定させる
			ImGui::Render();
#endif

			//SRV用のヒープ
			ID3D12DescriptorHeap* descriptorHeap[] = { DirectXCommon::GetInstance()->GetSrvDescriptorHeap() };
			DirectXCommon::GetInstance()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);

			DirectXCommon::GetInstance()->GetCommandList()->RSSetViewports(1, &camera3D->GetViewport());
			DirectXCommon::GetInstance()->GetCommandList()->RSSetScissorRects(1, &camera3D->GetScissorRect());
			//RootSignatureを設定。PSOとは別途設定が必要
			DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(graphicsPipeline->GetRootSignature());
			DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(graphicsPipeline->GetGraphicsPipelineState());

			//三角形の描画処理
			sphere->Draw();

			//スプライトの描画
			sprite->Draw();

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
		//DirectX12基盤の終了
		DirectXCommon::GetInstance()->Finalize();

		//ウィンドウズアプリケーションの終了
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
