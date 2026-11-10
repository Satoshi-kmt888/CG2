#include "App/StarEngine.h"

#include "2d/Sprite.h"
#include "2d/SpriteCommon.h"
#include "3d/Camera.h"
#include "3d/DebugCamera.h"
#include "3d/DirectionalLight.h"
#include "3d/Model.h"
#include "3d/Particle.h"
#include "3d/ParticleSystem.h"
#include "Debugger/D3D12ResourceLeakChecker.h"
#include "Graphics/GraphicsSystem.h"
#include "Input/Input.h"
#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include "Pipeline/PipelineObject3D.h"
#include "Pipeline/PipelineParticle.h"

#include <array>
#include <imgui.h>
#include <memory>
#include <random>
#include <Windows.h>

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//リークチェック(デストラクタでチェックが入る)
	D3D12ResourceLeakChecker leak_check;

	//エンジンの初期化
	StarEngine::Initialize(L"CG3");

	//ランダム生成器
	std::random_device seed_generator;
	std::mt19937 random_engine(seed_generator());

	//Δタイム
	const float k_delta_time = 1.0f / 60.0f;

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
	Transform sprite_transform = {
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f }
	};
	auto sprite = Sprite::Create();

	//モデル
	Transform model_transform = {
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f }
	};
	auto model = Model::CreatePlane();
	model->SetLightType(2);
	Vector4 model_color = { 1.0f, 1.0f, 1.0f, 1.0f };

	//パーティクル
	const uint32_t k_particle_count = 10;

	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> dist_color(0.0f, 1.0f);
	std::uniform_real_distribution<float> dist_time(1.0f, 3.0f);

	std::vector<Particle> particles(k_particle_count);
	for (auto& particle : particles) {
		particle.transform.translation = { distribution(random_engine), distribution(random_engine), distribution(random_engine) };
		particle.transform.rotation = { 0.0f, 0.0f, 0.0f };
		particle.transform.scale = { 1.0f, 1.0f, 1.0f };

		particle.velocity = { distribution(random_engine), distribution(random_engine), distribution(random_engine) };
		particle.color = { dist_color(random_engine), dist_color(random_engine) , dist_color(random_engine), 1.0f };
		particle.life_time = dist_time(random_engine);
		particle.current_time = 0.0f;
	}

	ParticleSystem particle_system{};
	particle_system.Initialize(k_particle_count);
	bool is_move_particle = false;

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
			ImGui::DragFloat3("transform: translation", &sprite_transform.translation.x, 0.01f);
			ImGui::DragFloat3("transform: rotation", &sprite_transform.rotation.x, 0.01f);
			ImGui::DragFloat3("transform: scale", &sprite_transform.scale.x, 0.01f);
		}

		if (ImGui::CollapsingHeader("Model")) {
			//モデルのトランスフォーム
			ImGui::DragFloat3("transform: translation", &model_transform.translation.x, 0.01f);
			ImGui::DragFloat3("transform: rotation", &model_transform.rotation.x, 0.01f);
			ImGui::DragFloat3("transform: scale", &model_transform.scale.x, 0.01f);

			//モデルの色
			ImGui::ColorEdit4("color", &model_color.x);
			model->SetColor(model_color);
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

		for (auto& particle : particles) {
			particle.transform.translation += particle.velocity * k_delta_time;
			particle.current_time += k_delta_time;
		}
		particle_system.SetParticle(particles);

		//====================
		// ↑更新処理↑
		//====================

		//====================
		// ↓描画処理↓
		//====================

		auto command_list = GraphicsSystem::GetInstance()->GetCommandList();

		//モデル
		pipeline_object3d.Bind(command_list);
		command_list->SetGraphicsRootConstantBufferView(3, light->GetGPUVirtualAddress());
		model->Draw(model_transform, camera->GetViewProjMatrix());

		//スプライト
		sprite_common->SetPipelineState(command_list);
		command_list->SetGraphicsRootConstantBufferView(3, light->GetGPUVirtualAddress());
		sprite->Draw(sprite_transform, camera_2d.GetViewProjMatrix());

		//パーティクル
		pipeline_particle.Bind(command_list);
		particle_system.Draw(camera->GetViewProjMatrix());

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
