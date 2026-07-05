#include "FrameWork/StarEngine.h"

#include "Audio/AudioManager.h"
#include "Camera/Camera.h"
#include "Logging/D3D12ResourceLeakChecker.h"
#include "Input/InputManager.h"
#include "Graphics/Model.h"
#include "Math/Vector3.h"
#include "Camera/DebugCamera.h"

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

	//カメラの生成・初期化
	auto mainCamera = std::make_unique<Camera>(1280.0f, 720.0f);
	auto debugCamera = std::make_unique<DebugCamera>();
	Camera const* activeCamera = mainCamera.get();
	bool enableDebug = false;

	//球を生成・初期化
	std::unique_ptr<Model> model = Model::CreateSphere("resources/uvChecker.png");

	//ウィンドウの×ボタンが押されるまでループ
	while (StarEngine::ProcessMessage()) {
		//フレーム開始処理
		StarEngine::BeginFrame();

		//====================
		// ↓更新処理↓
		//====================

		//カメラの更新
		if (InputManager::GetInstance()->TriggerKey(DIK_F1)) {
			enableDebug = !enableDebug;
			activeCamera = enableDebug ? &debugCamera->GetCamera() : mainCamera.get();
		}

		if (enableDebug) {
			debugCamera->Update();
		} else {
			mainCamera->Update();
		}

		//球の更新
		model->Update(activeCamera->GetViewProjMatrix());

		//音を鳴らす
		if (InputManager::GetInstance()->TriggerKey(DIK_SPACE)) {
			AudioManager::GetInstance()->SoundPlayWave(soundData1);
		}

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
