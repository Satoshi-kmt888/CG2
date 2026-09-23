#include "App/StarEngine.h"

#include "Debugger/D3D12ResourceLeakChecker.h"
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/RootSignature.h"
#include "Graphics/ShaderCompiler.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "Scene/Camera.h"
#include "Scene/DirectionalLight.h"
#include "Scene/Model.h"

#include <imgui.h>
#include <memory>
#include <Windows.h>

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//リークチェック(デストラクタでチェックが入る)
	D3D12ResourceLeakChecker leakCheck;

	//エンジンの初期化
	StarEngine::Initialize();

	//ルートシグネチャ
	auto rootSignature = std::make_unique<RootSignature>();
	rootSignature->AddCBV(0, D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature->AddCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.NumDescriptors = 1;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::vector<D3D12_DESCRIPTOR_RANGE> ranges = { descriptorRange };
	rootSignature->AddDescriptorTable(ranges, D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature->AddCBV(1, D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature->AddStaticSampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_SHADER_VISIBILITY_PIXEL
	);
	if (!rootSignature->Build(GraphicsSystem::GetInstance()->GetDevice())) {
		return false;
	}

	//コンパイラ
	auto compiler = std::make_unique<ShaderCompiler>();
	compiler->Initialize();
	auto vsBlob = compiler->Compile(L"Shader/Object3d.VS.hlsl", L"vs_6_0");
	auto psBlob = compiler->Compile(L"Shader/Object3d.PS.hlsl", L"ps_6_0");
	if (!vsBlob || !psBlob) return false;

	//パイプライン
	auto pipeline = std::make_unique<GraphicsPipeline>();
	pipeline->SetRootSignature(rootSignature.get())
		.SetVertexShader(vsBlob.Get())
		.SetPixelShader(psBlob.Get())
		.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT, 0)
		.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 0)
		.AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 0)
		.SetBlendState(BlendStates::Alpha())
		.SetRasterizerState(RasterizerStates::BackCull())
		.SetDepthStencilState(DepthStencilStates::Default());

	if (!pipeline->Build(GraphicsSystem::GetInstance()->GetDevice())) {
		return false;
	}

	//カメラ
	auto camera = std::make_unique<Camera>(1280.0f, 720.0f);

	//ライト
	auto light = std::make_unique<DirectionalLight>();
	light->Initialize();
	Vector4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	//モデル
	auto plane = Model::CreateFromOBJ("plane.obj");
	plane->SetLightType(2);
	Transform transformPlane{};
	Vector4 planeColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	//ウィンドウの×ボタンが押されるまでループ
	while (StarEngine::ProcessMessage()) {
		//フレーム開始処理
		StarEngine::BeginFrame();

		//====================
		// ↓更新処理↓
		//====================

		//カメラを更新
		camera->Update();
		//ライトを更新
		light->Update();

		ImGui::Begin("Settings");

		//平面モデルのカラー
		ImGui::ColorEdit4("plane color", &planeColor.x);
		plane->SetColor(planeColor);

		//ライト
		ImGui::ColorEdit4("light color", &lightColor.x);
		light->SetColor(lightColor);

		ImGui::End();

		//====================
		// ↑更新処理↑
		//====================

		//====================
		// ↓描画処理↓
		//====================

		auto commandList = GraphicsSystem::GetInstance()->GetCommandList();
		commandList->SetGraphicsRootSignature(rootSignature->Get());
		commandList->SetPipelineState(pipeline->Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->SetGraphicsRootConstantBufferView(3, light->GetGPUVirtualAddress());

		//平面モデルを描画
		plane->Draw(transformPlane, camera->GetViewProjMatrix());

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
