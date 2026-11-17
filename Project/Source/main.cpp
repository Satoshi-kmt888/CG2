#include "App/StarEngine.h"

#include "2d/Sprite.h"
#include "2d/SpriteCommon.h"
#include "3d/Camera.h"
#include "3d/DebugCamera.h"
#include "3d/DirectionalLight.h"
#include "3d/Emitter.h"
#include "3d/Model.h"
#include "3d/Particle.h"
#include "3d/ParticleSystem.h"
#include "Debugger/D3D12ResourceLeakChecker.h"
#include "Graphics/GraphicsSystem.h"
#include "Input/Input.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/WorldTransform.h"

#include "Pipeline/PipelineObject2D.h"
#include "Pipeline/PipelineObject3D.h"
#include "Pipeline/PipelineParticle.h"

#include <array>
#include <imgui.h>
#include <list>
#include <memory>
#include <random>
#include <Windows.h>

Particle MakeParticle(std::mt19937& random_engine, const Vector3& position);

std::list<Particle> Emit(const Emitter& emitter, std::mt19937& random_engine);

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//リークチェック(デストラクタでチェックが入る)
	D3D12ResourceLeakChecker leak_check;

	//エンジンの初期化
	StarEngine::Initialize(L"LE2B_04_カマタ_サトシ");

	//ランダム生成器
	std::random_device seed_generator;
	std::mt19937 random_engine(seed_generator());

	//デバイス
	auto device = GraphicsSystem::GetInstance()->GetDevice();

	//Δタイム
	const float k_delta_time = 1.0f / 60.0f;

	//スプライト描画前処理用
	auto sprite_common = SpriteCommon::GetInstance();
	if (!sprite_common->Initialize(device)) {
		return false;
	}

	//Object3Dパイプライン
	PipelineObject3D pipeline_object3d{};
	if (!pipeline_object3d.Initialize(device)) {
		return false;
	}

	//パーティクルパイプライン
	PipelineParticle pipeline_particle{};
	if (!pipeline_particle.Initialize(device)) {
		return false;
	}

	//カメラ
	Camera camera{ 1280.0f, 720.0f };

	//デバッグカメラ
	DebugCamera debug_camera{};
	bool is_debug_camera_enabled = false;

	//ライト
	auto light = std::make_unique<DirectionalLight>();
	light->Initialize();
	Vector4 color_light = { 1.0f, 1.0f, 1.0f, 1.0f };

	//スプライト
	auto sprite = Sprite::Create("uvChecker.png");
	sprite->SetPosition(Vector2{ 0.0f, 0.0f });
	sprite->SetSize(Vector2{ 256.0f, 256.0f });

	//モデル
	auto model = Model::CreatePlane("uvChecker.png");
	WorldTransform model_world_transform = {
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f }
	};

	//パーティクル
	std::list<Particle> particles;
	auto particle_system = std::make_unique<ParticleSystem>();
	particle_system->Initialize();
	//エミッター
	Emitter emitter{};
	emitter.world_transform.translation = {0.0f, 0.0f, 0.0f};
	emitter.world_transform.rotation = {0.0f, 0.0f, 0.0f};
	emitter.world_transform.scale = {1.0f, 1.0f, 1.0f};
	emitter.count = 3;
	emitter.frequency = 0.5f;
	emitter.frequency_time = 0.0f;

	//ウィンドウの×ボタンが押されるまでループ
	while (StarEngine::ProcessMessage()) {
		//フレーム開始処理
		StarEngine::BeginFrame();

		//=====================
		// ↓更新処理ここから↓
		//=====================

#ifdef USE_IMGUI
		//---------------------
		//課題が変わってもあるといい設定集はここ
		//---------------------

		ImGui::Begin("Settings");

		if (ImGui::CollapsingHeader("Camera")) {
			Vector3 translation = camera.GetTranslation();
			ImGui::DragFloat3("transform: translation###Camera::translation", &translation.x, 0.01f);
			camera.SetTranslation(translation);

			Vector3 rotation = camera.GetRotation();
			ImGui::DragFloat3("transform: rotation###Camera::rotation", &rotation.x, 0.01f);
			camera.SetRotation(rotation);
		}

		if (ImGui::CollapsingHeader("DirectionalLight")) {
			//ライトの色
			ImGui::ColorEdit4("color###DirectionalLight::color ", &color_light.x);
			light->SetColor(color_light);
		}

		ImGui::End();

		//---------------------
		//課題ごとの調整項目はここ
		//---------------------

		ImGui::Begin("Debug");

		if (ImGui::CollapsingHeader("Model")) {
			ImGui::DragFloat3("translation###ModelTranslation", &model_world_transform.translation.x, 0.01f);
			ImGui::DragFloat3("rotation###ModelRotation", &model_world_transform.rotation.x, 0.01f);
			ImGui::DragFloat3("scale###ModelScale", &model_world_transform.scale.x, 0.01f);
		}

		if (ImGui::CollapsingHeader("Particle")) {
			ImGui::DragFloat3("emitter translation###EmitterTranslation", &emitter.world_transform.translation.x, 0.01f);
			ImGui::DragFloat3("emitter rotation###EmitterRotation", &emitter.world_transform.rotation.x, 0.01f);
			ImGui::DragFloat3("emitter scale###EmitterScale", &emitter.world_transform.scale.x, 0.01f);

			if (ImGui::Button("Emit Particle")) {
				particles.splice(particles.end(), Emit(emitter, random_engine));
			}
		}

		ImGui::End();
#endif

#ifdef _DEBUG
		if (Input::GetInstance()->GetKeyboard().Trigger(DIK_F1)) {
			is_debug_camera_enabled = !is_debug_camera_enabled;
		}
#endif
		/*--- カメラを更新 ---*/

		if (is_debug_camera_enabled) {
			debug_camera.Update();
			camera.SetWorldMatrix(debug_camera.GetCamera().GetWorldMatrix());
			camera.SetViewProjectionMatrix(debug_camera.GetCamera().GetViewProjMatrix());
		} else {
			camera.Update();
		}

		/*--- ライトを更新 ---*/

		light->Update();

		/*--- オブジェクトを更新 ---*/

		emitter.frequency_time += k_delta_time;
		if (emitter.frequency <= emitter.frequency_time) {
			particles.splice(particles.end(), Emit(emitter, random_engine));
			emitter.frequency_time -= emitter.frequency;
		}

		for (auto particle_iterator = particles.begin(); particle_iterator != particles.end(); ++particle_iterator) {
			particle_iterator->transform.translation += particle_iterator->velocity * k_delta_time;
			particle_iterator->current_time += k_delta_time;
		}
		particle_system->SetParticle(particles);
		particle_system->Update();

		//=====================
		// ↑更新処理ここまで↑
		//=====================

		//=====================
		// ↓描画処理ここから↓
		//=====================

		auto command_list = GraphicsSystem::GetInstance()->GetCommandList();

		//=====================================================
		// モデル
		//=====================================================

		pipeline_object3d.Bind(command_list);
		command_list->SetGraphicsRootConstantBufferView(3, light->GetGPUVirtualAddress());

		//モデルを描画
		model->Draw(model_world_transform, camera.GetViewProjMatrix());

		//=====================================================
		// パーティクル
		//=====================================================

		pipeline_particle.Bind(command_list);

		particle_system->Draw(&camera);

		//=====================================================
		// スプライト
		//=====================================================

		sprite_common->SetPipelineState(command_list);
		command_list->SetGraphicsRootConstantBufferView(3, light->GetGPUVirtualAddress());

		//スプライトを描画
		sprite->Draw();

		//=====================
		// ↑描画処理ここまで↑
		//=====================

		//フレーム終了処理
		StarEngine::EndFrame();
	}

	sprite_common->Finalize();
	//エンジンの終了
	StarEngine::Finalize();

	return 0;
}

Particle MakeParticle(std::mt19937& random_engine, const Vector3& position) {
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> dist_scale(1.0f, 3.0f);
	Vector3 random_position = { distribution(random_engine), distribution(random_engine) , distribution(random_engine) };
	Particle particle;
	particle.transform.scale.x = dist_scale(random_engine);
	particle.transform.scale = { particle.transform.scale.x, particle.transform.scale.x , 1.0f };
	particle.transform.rotation = { 0.0f, 0.0f, 0.0f };
	particle.transform.translation = position + random_position;
	particle.velocity = { distribution(random_engine) , distribution(random_engine) ,distribution(random_engine) };

	std::uniform_real_distribution<float> dist_color(0.0f, 1.0f);    //色
	std::uniform_real_distribution<float> dist_time(1.0f, 3.0f);     //生存時間

	particle.color = { dist_color(random_engine), dist_color(random_engine), dist_color(random_engine), 1.0f };
	particle.life_time = dist_time(random_engine);

	particle.current_time = 0.0f;
	return particle;
}

std::list<Particle> Emit(const Emitter& emitter, std::mt19937& random_engine) {
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeParticle(random_engine, emitter.world_transform.translation));
	}

	return particles;
}
