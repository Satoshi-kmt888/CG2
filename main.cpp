#include "StarEngine.h"

#include "AudioManager.h"
#include "Camera.h"
#include "D3D12ResourceLeakChecker.h"
#include "Model.h"
#include "Vector3.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

#include <memory>

#include <sal.h>
#include <Windows.h>

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//リークチェック(デストラクタでチェックが入る)
	D3D12ResourceLeakChecker leakCheck;

	//エンジンの初期化
	StarEngine::Initialize();


	//音声
	AudioManager::SoundData soundData1 = AudioManager::GetInstance()->SoundLoadWave("resources/fanfare.wav");
	AudioManager::GetInstance()->SoundPlayWave(soundData1);


	//カメラの生成・初期化
	std::unique_ptr<Camera> camera3D = std::make_unique<Camera>(1280.0f, 720.0f);

	//球を生成・初期化
	std::unique_ptr<Model> model = Model::CreateSphere("resources/uvChecker.png");

	//ウィンドウの×ボタンが押されるまでループ
	while (StarEngine::ProcessMessage()) {
		//フレーム開始処理
		StarEngine::BeginFrame();

		//====================
		// ↓更新処理↓
		//====================

#ifdef _DEBUG
		//--- カメラ ---

		//回転
		Vector3 camera3DRotation = camera3D->GetRotation();
		ImGui::DragFloat3("Camera3D Rotation", &camera3DRotation.x, 0.01f);
		camera3D->SetRotation(camera3DRotation);

		//平行移動
		Vector3 camera3DTranslation = camera3D->GetTranslation();
		ImGui::DragFloat3("Camera3D Translation", &camera3DTranslation.x, 0.01f);
		camera3D->SetTranslation(camera3DTranslation);

		//--- モデル ---

		Vector3 planeRotation = model->GetRotation();
		ImGui::DragFloat3("Plane Rotation", &planeRotation.x, 0.01f);
		model->SetRotation(planeRotation);
#endif

		//カメラの更新
		camera3D->Update();

		//球の更新
		model->Update(camera3D->GetViewProjMatrix());

		//====================
		// ↑更新処理↑
		//====================

		//====================
		// ↓描画処理↓
		//====================

		//球の描画
		model->Draw();

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
