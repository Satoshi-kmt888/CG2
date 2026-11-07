#include "App/StarEngine.h"

#include "3d/Camera.h"
#include "3d/DebugCamera.h"
#include "3d/DirectionalLight.h"
#include "3d/Model.h"
#include "3d/Particle.h"
#include "Debugger/D3D12ResourceLeakChecker.h"
#include "Graphics/D3D12Utility.h"
#include "Graphics/GraphicsSystem.h"
#include "Input/Input.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "Pipeline/GraphicsPipeline.h"
#include "Pipeline/RootSignature.h"
#include "Pipeline/ShaderCompiler.h"

#include <array>
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

	D3D12_DESCRIPTOR_RANGE descriptor_range_texture{};
	descriptor_range_texture.BaseShaderRegister = 0;
	descriptor_range_texture.NumDescriptors = 1;
	descriptor_range_texture.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptor_range_texture.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptor_range_instancing{};
	descriptor_range_instancing.BaseShaderRegister = 0;
	descriptor_range_instancing.NumDescriptors = 1;
	descriptor_range_instancing.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptor_range_instancing.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	std::vector<D3D12_DESCRIPTOR_RANGE> ranges = { descriptor_range_texture, descriptor_range_instancing };
	root_signature->AddCBV(0, D3D12_SHADER_VISIBILITY_PIXEL);
	root_signature->AddCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	root_signature->AddDescriptorTable({ descriptor_range_texture }, D3D12_SHADER_VISIBILITY_PIXEL);
	root_signature->AddDescriptorTable({ descriptor_range_instancing }, D3D12_SHADER_VISIBILITY_VERTEX);
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
	auto vs_blob = compiler->Compile(L"Resource/Shader/Particle.VS.hlsl", L"vs_6_0");
	auto ps_blob = compiler->Compile(L"Resource/Shader/Particle.PS.hlsl", L"ps_6_0");
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

	//パーティクル
	const UINT k_particle_count = 10;
	Particle particle{};
	particle.Initialize(k_particle_count);
	std::array<Transform, k_particle_count> transform_particles;
	for (uint32_t i = 0; i < k_particle_count; ++i) {
		transform_particles[i].scale = { 1.0f, 1.0f, 1.0f };
		transform_particles[i].rotation = { 0.0f, 0.0f, 0.0f };
		transform_particles[i].translation = { static_cast<float>(i) * 0.1f, static_cast<float>(i) * 0.1f, static_cast<float>(i) * 0.1f };
	}


	D3D12_SHADER_RESOURCE_VIEW_DESC instancing_srv_desc{};
	instancing_srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	instancing_srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancing_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancing_srv_desc.Buffer.FirstElement = 0;
	instancing_srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancing_srv_desc.Buffer.NumElements = k_particle_count;
	instancing_srv_desc.Buffer.StructureByteStride = sizeof(Particle::TransformationMatrix);
	const auto allocation = DescriptorManager::GetInstance()->Allocate(DescriptorType::SRV_CBV_UAV);
	const D3D12_CPU_DESCRIPTOR_HANDLE instancing_srv_handle_cpu = allocation.cpuHandle;
	const D3D12_GPU_DESCRIPTOR_HANDLE instancing_srv_handle_gpu = allocation.gpuHandle;
	GraphicsSystem::GetInstance()->GetDevice()->CreateShaderResourceView(particle.GetResource(), &instancing_srv_desc, instancing_srv_handle_cpu);


	//ウィンドウの×ボタンが押されるまでループ
	while (StarEngine::ProcessMessage()) {
		//フレーム開始処理
		StarEngine::BeginFrame();

		//====================
		// ↓更新処理↓
		//====================

#ifdef _DEBUG
		if (Input::GetInstance()->GetKeyboard().Trigger(DIK_F1)) {
			is_debug_camera_enabled = !is_debug_camera_enabled;
			camera = is_debug_camera_enabled ? &debug_camera.GetCamera() : &main_camera;
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
		commandList->SetGraphicsRootConstantBufferView(4, light->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(3, instancing_srv_handle_gpu);

		//平面モデルを描画
		particle.Draw(transform_particles, camera->GetViewProjMatrix());

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
