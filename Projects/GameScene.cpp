#include "GameScene.h"

#include "Graphics/GraphicsSystem.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

void GameScene::Initialize() {
	//--- カメラの初期化 ---

	//2Dカメラ
	camera2D_.Initialize(1280.0f, 720.0f);
	camera2D_.SetProjectionType(ProjectionType::Orthographic);

	//3Dカメラ
	camera3D_.Initialize(1280.0f, 720.0f);

	//--- ライトの初期化 ---

	//平行ライト
	directionalLight_.Initialize();

	//--- スプライトの初期化 ---

	sprite_ = Sprite::Create();
	transformSprite_.scale = { 480.0f, 270.0f, 1.0f };
	transformSprite_.translation = { transformSprite_.scale.x / 2.0f, transformSprite_.scale.y / 2.0f, 1.0f };

	//--- 平面モデルの初期化 ---

	plane_ = Model::CreateFromOBJ("plane.obj");

	//--- 球モデルの初期化 ---

	sphere_ = Model::CreateSphere();
}

void GameScene::Update() {
	camera2D_.Update();
	camera3D_.Update();

	directionalLight_.Update();
}

void GameScene::Draw() {
	//コマンドリストの取得
	auto commandList = GraphicsSystem::GetInstance()->GetCommandList();

	//ライトの設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLight_.GetGPUVirtualAddress());

	//--- 平面モデルの描画 ---

	plane_->Draw(transformPlane_, camera3D_.GetViewProjMatrix());

	//--- 球モデルの描画 ---

	//sphere_->Draw(transformSphere_, camera3D_.GetViewProjMatrix());

	//--- スプライトの描画 ---

	sprite_->Draw(transformSprite_, camera2D_.GetViewProjMatrix());
}

void GameScene::ControlImGui() {

}
