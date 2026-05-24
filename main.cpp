#include "StarEngine.h"

#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Sprite.h"
#include "Transform.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

#include <memory>

#include <sal.h>
#include <Windows.h>

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//エンジンの初期化
	StarEngine::Initialize();

	//カメラの生成・初期化
	std::unique_ptr<Camera> camera2D = std::make_unique<Camera>(1280.0f, 720.0f);
	camera2D->SetProjectionType(ProjectionType::Orthographic);
	std::unique_ptr<Camera> camera3D = std::make_unique<Camera>(1280.0f, 720.0f);

	//スプライト(矩形)を生成・初期化
	std::unique_ptr<Sprite> quadSprite = std::make_unique<Sprite>();
	quadSprite->Initialize("resources/uvChecker.png");

	//球を生成・初期化
	std::unique_ptr<Model> sphereModel = Model::CreateSphere("resources/uvChecker.png");

	//ウィンドウの×ボタンが押されるまでループ
	while (StarEngine::ProcessMessage()) {
		//フレーム開始処理
		StarEngine::BeginFrame();

		//====================
		// ↓更新処理↓
		//====================

#ifdef _DEBUG
		//カメラ
		Vector3 camera3DTranslation = camera3D->GetTranslation();
		Vector3 camera2DTranslation = camera2D->GetTranslation();

		ImGui::DragFloat3("Camera3D Translation", &camera3DTranslation.x, 0.01f);
		ImGui::DragFloat3("Camera2D Translation", &camera2DTranslation.x, 0.1f);

		camera3D->SetTranslation(camera3DTranslation);
		camera2D->SetTranslation(camera2DTranslation);

		//スプライト
		Material& spriteMaterial = quadSprite->GetMaterial();
		Transform uvTransform = spriteMaterial.GetUVTransform();

		ImGui::DragFloat2("UVScale", &uvTransform.scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &uvTransform.rotation.z);
		ImGui::DragFloat2("UVTranslate", &uvTransform.translation.x, 0.01f, -10.0f, 10.0f);

		spriteMaterial.SetUVTransform(uvTransform);
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
	quadSprite.reset();
	sphereModel.reset();

	//エンジンの終了
	StarEngine::Finalize();

	return 0;
}
