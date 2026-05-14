#include "StarEngine.h"

#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Sprite.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif

#include <memory>

#include <Windows.h>
#include <sal.h>

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//エンジンの初期化
	StarEngine::Initialize();

	//カメラの生成・初期化
	std::unique_ptr<Camera> camera2D = std::make_unique<Camera>();
	camera2D->Initialize(1280, 720);
	camera2D->SetProjectionType(ProjectionType::Orthographic);
	std::unique_ptr<Camera> camera3D = std::make_unique<Camera>();
	camera3D->Initialize(1280, 720);

	//スプライト(矩形)を生成・初期化
	std::unique_ptr<Mesh> quadMesh = Mesh::CreateQuad();
	std::unique_ptr<Sprite> quadSprite = std::make_unique<Sprite>();
	quadSprite->Initialize();
	quadSprite->SetMesh(quadMesh.get());
	quadSprite->SetTexture("resources/uvChecker.png");

	//球を生成・初期化
	std::unique_ptr<Mesh> sphereMesh = Mesh::CreateSphere();
	std::unique_ptr<Model> sphereModel = std::make_unique<Model>();
	sphereModel->Initialize();
	sphereModel->SetMesh(sphereMesh.get());
	sphereModel->SetTexture("resources/uvChecker.png");

	//ウィンドウの×ボタンが押されるまでループ
	while (StarEngine::ProcessMessage()) {
		//フレーム開始処理
		StarEngine::BeginFrame();

		//====================
		// ↓更新処理↓
		//====================

#ifdef USE_IMGUI
		ImGui::DragFloat3("Camera3D Translation", &camera3D->GetTranslation().x, 0.01f);
		ImGui::DragFloat3("Camera2D Translation", &camera2D->GetTranslation().x, 0.1f);
#endif

		//カメラの更新
		camera2D->Update();
		camera3D->Update();

		//スプライトの更新
		quadSprite->Update(camera2D->GetViewProjMatrix());

		//球の更新
		sphereModel->Update(camera3D->GetViewProjMatrix());

		//====================
		// ↑更新処理↑
		//====================

		//====================
		// ↓描画処理↓
		//====================

		//スプライトの描画
		quadSprite->Draw();

		//球の描画
		sphereModel->Draw();

		//====================
		// ↑描画処理↑
		//====================

		//フレーム終了処理
		StarEngine::EndFrame();
	}

	//生成したオブジェクトを明示的にリセット(リークチェックに引っかかるため)
	quadMesh.reset();
	quadSprite.reset();
	sphereMesh.reset();
	sphereModel.reset();

	//エンジンの終了
	StarEngine::Finalize();

	return 0;
}
