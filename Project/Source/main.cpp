#include "App/StarEngine.h"

#include "2d/Sprite.h"
#include "2d/SpriteCommon.h"
#include "3d/Camera.h"
#include "3d/DebugCamera.h"
#include "3d/DirectionalLight.h"
#include "3d/Model.h"
#include "3d/Particle.h"
#include "Debugger/D3D12ResourceLeakChecker.h"
#include "Graphics/GraphicsSystem.h"
#include "Input/Input.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"

#include "Pipeline/PipelineObject3D.h"
#include "Pipeline/PipelineParticle.h"

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

	PipelineObject3D pipeline_object3d{};
	if (!pipeline_object3d.Initialize(GraphicsSystem::GetInstance()->GetDevice())) {
		return false;
	}

	PipelineParticle pipeline_particle{};
	if (!pipeline_particle.Initialize(GraphicsSystem::GetInstance()->GetDevice())) {
		return false;
	}

	//メインカメラ(切り替える)
	Camera main_camera{ 1280.0f, 720.0f };

	//デバッグカメラ(切り替える)
	DebugCamera debug_camera{};
	bool is_debug_camera_enabled = false;

	//カメラ
	Camera const* camera = &main_camera;
	Camera camera_2d{ 1280.0f, 720.0f };

	//ライト
	auto light = std::make_unique<DirectionalLight>();
	light->Initialize();
	Vector4 color_light = { 1.0f, 1.0f, 1.0f, 1.0f };

	auto sprite_common = SpriteCommon::GetInstance();
	if (!sprite_common->Initialize(GraphicsSystem::GetInstance()->GetDevice())) {
		return false;
	}

	//スプライト
	Transform transform_sprite = {
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f }
	};
	auto sprite = Sprite::Create();

	//モデル
	//Transform transform_model = {
	//	{ 1.0f, 1.0f, 1.0f },
	//	{ 0.0f, 0.0f, 0.0f },
	//	{ 0.0f, 0.0f, 0.0f }
	//};
	//auto model = Model::CreatePlane();
	//model->SetLightType(2);
	//Vector4 color_model = { 1.0f, 1.0f, 1.0f, 1.0f };

	//パーティクル
	//const uint32_t k_particle_count = 10;
	//Particle particle{};
	//particle.Initialize(k_particle_count);
	//std::array<Transform, k_particle_count> transform_particles;
	//for (uint32_t i = 0; i < k_particle_count; ++i) {
	//	transform_particles[i].scale = { 1.0f, 1.0f, 1.0f };
	//	transform_particles[i].rotation = { 0.0f, 0.0f, 0.0f };
	//	transform_particles[i].translation = { static_cast<float>(i) * 0.1f, static_cast<float>(i) * 0.1f, static_cast<float>(i) * 0.1f };
	//}

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

#ifdef USE_IMGUI
		//---------------------
		//課題が変わってもあるといい設定集はここ
		//---------------------

		ImGui::Begin("Settings");

		if (ImGui::CollapsingHeader("DirectionalLight")) {
			//ライトの色
			ImGui::ColorEdit4("color", &color_light.x);
			light->SetColor(color_light);
		}

		ImGui::End();

		//---------------------
		//課題ごとの調整項目はここ
		//---------------------

		ImGui::Begin("Debug");

		if (ImGui::CollapsingHeader("Sprite")) {
			ImGui::DragFloat3("transform: translation", &transform_sprite.translation.x, 0.01f);
			ImGui::DragFloat3("transform: rotation", &transform_sprite.rotation.x, 0.01f);
			ImGui::DragFloat3("transform: scale", &transform_sprite.scale.x, 0.01f);
		}

		if (ImGui::CollapsingHeader("Model")) {
			//モデルのトランスフォーム
			//ImGui::DragFloat3("transform: translation", &transform_model.translation.x, 0.01f);
			//ImGui::DragFloat3("transform: rotation", &transform_model.rotation.x, 0.01f);
			//ImGui::DragFloat3("transform: scale", &transform_model.scale.x, 0.01f);

			////モデルの色
			//ImGui::ColorEdit4("color", &color_model.x);
			//model->SetColor(color_model);
		}

		ImGui::End();
#endif

		if (is_debug_camera_enabled) {
			debug_camera.Update();
		} else {
			main_camera.Update();
		}
		camera_2d.Update();

		//ライトを更新
		light->Update();

		//====================
		// ↑更新処理↑
		//====================

		//====================
		// ↓描画処理↓
		//====================

		auto command_list = GraphicsSystem::GetInstance()->GetCommandList();
		//pipeline_object3d.Bind(command_list);

		//model->Draw(transform_model, camera->GetViewProjMatrix());

		//pipeline_particle.Bind(command_list);

		//particle.Draw(transform_particles, camera->GetViewProjMatrix());

		sprite_common->SetPipelineState(command_list);
		command_list->SetGraphicsRootConstantBufferView(3, light->GetGPUVirtualAddress());
		sprite->Draw(transform_sprite, camera_2d.GetViewProjMatrix());

		//====================
		// ↑描画処理↑
		//====================

		//フレーム終了処理
		StarEngine::EndFrame();
	}

	sprite_common->Finalize();
	//エンジンの終了
	StarEngine::Finalize();

	return 0;
}
