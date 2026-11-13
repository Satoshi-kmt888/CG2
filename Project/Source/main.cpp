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

Particle MakeParticle(std::mt19937& random_engine);

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//リークチェック(デストラクタでチェックが入る)
	D3D12ResourceLeakChecker leak_check;

	//エンジンの初期化
	StarEngine::Initialize(L"LE2B_04_カマタ_サトシ");

	//ランダム生成器
	std::random_device seed_generator;
	std::mt19937 random_engine(seed_generator());

	//Δタイム
	const float k_delta_time = 1.0f / 60.0f;

	//Object3Dパイプライン
	PipelineObject3D pipeline_object3d{};
	if (!pipeline_object3d.Initialize(GraphicsSystem::GetInstance()->GetDevice())) {
		return false;
	}

	//パーティクルパイプライン
	PipelineParticle pipeline_particle{};
	if (!pipeline_particle.Initialize(GraphicsSystem::GetInstance()->GetDevice())) {
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

	//スプライト描画前処理用
	auto sprite_common = SpriteCommon::GetInstance();
	if (!sprite_common->Initialize(GraphicsSystem::GetInstance()->GetDevice())) {
		return false;
	}

	//スプライト
	WorldTransform sprite_transform = {
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f }
	};
	auto sprite = Sprite::Create();

	//モデル
	WorldTransform model_transform = {
		{ 100.0f, 100.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 50.0f }
	};
	auto model = Model::CreateFromOBJ("skysphere.obj");
	model->SetLightType(0);
	Vector4 model_color = { 0.0f, 0.0f, 0.0f, 1.0f };

	/*--- パーティクル ---*/

	//最大数
	int particle_count = 10;

	//パーティクル本体
	std::list<Particle> particles;

	//パーティクル管理システム
	ParticleSystem particle_system{};
	particle_system.Initialize();

	//ウィンドウの×ボタンが押されるまでループ
	while (StarEngine::ProcessMessage()) {
		//フレーム開始処理
		StarEngine::BeginFrame();

		//====================
		// ↓更新処理↓
		//====================

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

		//if (ImGui::CollapsingHeader("Sprite")) {
		//	ImGui::DragFloat2("transform: translation###Sprite::translation", &sprite_transform.translation.x, 1.0f);
		//	ImGui::DragFloat("transform: rotation###Sprite::rotation", &sprite_transform.rotation.z, 0.01f);
		//	ImGui::DragFloat2("transform: scale###Sprite::scale", &sprite_transform.scale.x, 1.0f);
		//}

		//if (ImGui::CollapsingHeader("Model")) {
		//	//モデルのトランスフォーム
		//	ImGui::DragFloat3("transform: translation###Model::translation", &model_transform.translation.x, 0.01f);
		//	ImGui::DragFloat3("transform: rotation###Model::rotation", &model_transform.rotation.x, 0.01f);
		//	ImGui::DragFloat3("transform: scale###Model::scale", &model_transform.scale.x, 0.01f);

		//	//モデルの色
		//	ImGui::ColorEdit4("color###Model::color", &model_color.x);
		//}

		if (ImGui::CollapsingHeader("Particle")) {
			ImGui::SliderInt("count", &particle_count, 1, 1024);

			if (ImGui::Button("Emit")) {
				for (int i = 0; i < particle_count; ++i) {
					particles.push_back(MakeParticle(random_engine));
				}
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

		model->SetColor(model_color);

		for (auto particle_iterator = particles.begin(); particle_iterator != particles.end(); ++particle_iterator) {
			particle_iterator->transform.translation += particle_iterator->velocity * k_delta_time;
			particle_iterator->current_time += k_delta_time;
		}
		particle_system.SetParticle(particles);

		//====================
		// ↑更新処理↑
		//====================

		//====================
		// ↓描画処理↓
		//====================

		auto command_list = GraphicsSystem::GetInstance()->GetCommandList();

		//---------------------
		// モデル描画ここから
		//---------------------

		//描画前処理
		pipeline_object3d.Bind(command_list);
		command_list->SetGraphicsRootConstantBufferView(3, light->GetGPUVirtualAddress());

		//モデルを描画
		model->Draw(model_transform, camera.GetViewProjMatrix());

		//---------------------
		// モデル描画ここまで
		//---------------------

		//---------------------
		// パーティクル描画ここから
		//---------------------

		//描画前処理
		pipeline_particle.Bind(command_list);

		//パーティクルを描画
		particle_system.Draw(&camera);

		//---------------------
		// パーティクル描画ここまで
		//---------------------

		//---------------------
		// スプライト描画ここから
		//---------------------

		//描画前処理
		sprite_common->SetPipelineState(command_list);
		command_list->SetGraphicsRootConstantBufferView(3, light->GetGPUVirtualAddress());

		//スプライトを描画
		//sprite->Draw(sprite_transform);

		//---------------------
		// スプライト描画ここまで
		//---------------------

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

Particle MakeParticle(std::mt19937& random_engine) {
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> dist_scale(1.0f, 3.0f);
	Particle particle;
	particle.transform.scale.x = dist_scale(random_engine);
	particle.transform.scale = { particle.transform.scale.x, particle.transform.scale.x , 1.0f };
	particle.transform.rotation = { 0.0f, 0.0f, 0.0f };
	particle.transform.translation = { distribution(random_engine), distribution(random_engine) , distribution(random_engine) };
	particle.velocity = { distribution(random_engine) , distribution(random_engine) ,distribution(random_engine) };

	std::uniform_real_distribution<float> dist_color(0.0f, 1.0f);    //色
	std::uniform_real_distribution<float> dist_time(1.0f, 5.0f);     //生存時間

	particle.color = { dist_color(random_engine), dist_color(random_engine), dist_color(random_engine), 1.0f };
	particle.life_time = dist_time(random_engine);

	particle.current_time = 0.0f;
	return particle;
}
