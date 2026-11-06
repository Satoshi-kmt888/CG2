#include "App/StarEngine.h"

#include "3d/Camera.h"
#include "3d/DebugCamera.h"
#include "3d/DirectionalLight.h"
#include "3d/Model.h"
#include "Debugger/D3D12ResourceLeakChecker.h"
#include "Graphics/GraphicsSystem.h"
#include "Input/Input.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "Pipeline/GraphicsPipeline.h"
#include "Pipeline/RootSignature.h"
#include "Pipeline/ShaderCompiler.h"

#include <imgui.h>
#include <memory>
#include <Windows.h>

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//リークチェック(デストラクタでチェックが入る)
	D3D12ResourceLeakChecker leak_check;

	//エンジンの初期化
	StarEngine::Initialize(L"CG3");

	//ルートシグネチャ
	auto root_signature = std::make_unique<RootSignature>();
	root_signature->AddCBV(0, D3D12_SHADER_VISIBILITY_PIXEL);
	root_signature->AddCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	D3D12_DESCRIPTOR_RANGE descriptor_range{};
	descriptor_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptor_range.NumDescriptors = 1;
	descriptor_range.BaseShaderRegister = 0;
	descriptor_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::vector<D3D12_DESCRIPTOR_RANGE> ranges = { descriptor_range };
	root_signature->AddDescriptorTable(ranges, D3D12_SHADER_VISIBILITY_PIXEL);
	root_signature->AddCBV(1, D3D12_SHADER_VISIBILITY_PIXEL);
	root_signature->AddStaticSampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_SHADER_VISIBILITY_PIXEL
	);
	if (!root_signature->Build(GraphicsSystem::GetInstance()->GetDevice())) {
		return false;
	}

	//コンパイラ
	auto compiler = std::make_unique<ShaderCompiler>();
	compiler->Initialize();
	auto vs_blob = compiler->Compile(L"Resource/Shader/Object3d.VS.hlsl", L"vs_6_0");
	auto ps_blob = compiler->Compile(L"Resource/Shader/Object3d.PS.hlsl", L"ps_6_0");
	if (!vs_blob || !ps_blob) {
		return false;
	}

	//パイプライン
	auto pipeline = std::make_unique<GraphicsPipeline>();
	pipeline->SetRootSignature(root_signature.get())
		.SetVertexShader(vs_blob.Get())
		.SetPixelShader(ps_blob.Get())
		.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT, 0)
		.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 0)
		.AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 0)
		.SetBlendState(BlendStates::Alpha())
		.SetRasterizerState(RasterizerStates::BackCull())
		.SetDepthStencilState(DepthStencilStates::Default());

	if (!pipeline->Build(GraphicsSystem::GetInstance()->GetDevice())) {
		return false;
	}

	//メインカメラ(切り替える)
	Camera main_camera{ 1280.0f, 720.0f };

	//デバッグカメラ(切り替える)
	DebugCamera debug_camera{};
	bool is_debug_camera_enabled = false;

	//カメラ
	Camera const* camera = &main_camera;

	//ライト
	auto light = std::make_unique<DirectionalLight>();
	light->Initialize();
	Vector4 light_color = { 1.0f, 1.0f, 1.0f, 1.0f };

	//モデル
	auto model = Model::CreatePlane();
	model->SetLightType(2);
	Transform transform{};
	Vector4 model_color = { 1.0f, 1.0f, 1.0f, 1.0f };

	//ウィンドウの×ボタンが押されるまでループ
	while (StarEngine::ProcessMessage()) {
		//フレーム開始処理
		StarEngine::BeginFrame();

		//====================
		// ↓更新処理↓
		//====================

#ifdef DEVELOP
		if (Input::GetInstance()->GetKeyboard().Trigger(DIK_F1)) {
			isDebugCameraEnabled = !isDebugCameraEnabled;
			camera = isDebugCameraEnabled ? &debugCamera.GetCamera() : &mainCamera;
		}
#endif

		if (is_debug_camera_enabled) {
			debug_camera.Update();
		} else {
			main_camera.Update();
		}

		//ライトを更新
		light->Update();

#ifdef USE_IMGUI
		ImGui::Begin("Settings");

		//平面モデルのカラー
		ImGui::ColorEdit4("plane color", &model_color.x);
		model->SetColor(model_color);

		//ライト
		ImGui::ColorEdit4("light color", &light_color.x);
		light->SetColor(light_color);

		ImGui::Separator();

		//フェンスモデルのSRT
		ImGui::DragFloat3("model.translation", &transform.translation.x, 0.01f);
		ImGui::DragFloat3("model.rotation", &transform.rotation.x, 0.01f);
		ImGui::DragFloat3("model.scale", &transform.scale.x, 0.01f);

		ImGui::End();

		ImGui::Begin("Debug");
		ImGui::End();
#endif

		//====================
		// ↑更新処理↑
		//====================

		//====================
		// ↓描画処理↓
		//====================

		auto commandList = GraphicsSystem::GetInstance()->GetCommandList();
		commandList->SetGraphicsRootSignature(root_signature->Get());
		commandList->SetPipelineState(pipeline->Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->SetGraphicsRootConstantBufferView(3, light->GetGPUVirtualAddress());

		//平面モデルを描画
		model->Draw(transform, camera->GetViewProjMatrix());

		//====================
		// ↑描画処理↑
		//====================

		//フレーム終了処理
		StarEngine::EndFrame();
	}

	//エンジンの終了
	StarEngine::Finalize();

	return 0;
}
