#include "Core/StarEngine.h"

#include "Camera/Camera.h"
#include "Diagnostics/D3D12ResourceLeakChecker.h"
#include "Input/InputManager.h"
#include "Camera/DebugCamera.h"
#include "Render/Model.h"

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

	//カメラの生成・初期化
	auto mainCamera = std::make_unique<Camera>(1280.0f, 720.0f);
	auto debugCamera = std::make_unique<DebugCamera>();
	Camera const* activeCamera = mainCamera.get();
	bool enableDebug = false;

	auto model = Model::CreateFromOBJ("axis.obj");
	Transform transform = {};

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

		//====================
		// ↑更新処理↑
		//====================

		//====================
		// ↓描画処理↓
		//====================

		model->Draw(transform, activeCamera->GetViewProjMatrix());

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
