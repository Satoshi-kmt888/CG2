#include "StarEngine.h"

#include "Camera.h"
#include "Mesh.h"
#include "Model.h"

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

	//
	std::unique_ptr<Camera> camera3D = std::make_unique<Camera>();
	camera3D->Initialize(1280, 720);

	std::unique_ptr<Mesh> sphereMesh = Mesh::CreateSphere();

	std::unique_ptr<Model> sphereModel = std::make_unique<Model>();
	sphereModel->Initialize();
	sphereModel->SetMesh(sphereMesh.get());
	sphereModel->SetTexture("resources/uvChecker.png");

	//ウィンドウの×ボタンが押されるまでループ
	while (StarEngine::ProcessMessage()) {
		//フレーム開始処理
		StarEngine::BeginFrame();

		camera3D->Update();

		sphereModel->Update(camera3D->GetViewProjMatrix());

		sphereModel->Draw();

		//フレーム終了処理
		StarEngine::EndFrame();
	}

	//エンジンの終了
	StarEngine::Finalize();

	return 0;
}
