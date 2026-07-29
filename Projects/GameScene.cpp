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
}

void GameScene::Update() {
	camera2D_.Update();
	camera3D_.Update();

	directionalLight_.Update();

	ControlImGui();
}

void GameScene::Draw() {
	//コマンドリストの取得
	auto commandList = GraphicsSystem::GetInstance()->GetCommandList();

	//ライトの設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLight_.GetGPUVirtualAddress());

	for (auto& obj : objects_) {
		if (obj.model) {
			obj.model->Draw(obj.transform, camera3D_.GetViewProjMatrix());
		} else if (obj.sprite) {
			obj.sprite->Draw(obj.transform, camera2D_.GetViewProjMatrix());
		}
	}
}

void GameScene::ControlImGui() {
#ifdef _DEBUG
	ImGui::Begin("Settings");

	//==================================================
	// モデルの生成ボタン
	//==================================================

	const char* objectTypeNames[] = { "Sprite", "Plane", "Sphere" };

	ImGui::Combo("Model", &selectedObjectTypeIndex_, objectTypeNames, IM_ARRAYSIZE(objectTypeNames));

	if (ImGui::Button("Create")) {
		GameObject newObject;
		newObject.id = nextObjectID_++;
		newObject.type = static_cast<ObjectType>(selectedObjectTypeIndex_);

		newObject.transform = {};

		switch (newObject.type) {
		case ObjectType::kSprite:
			//スプライトを生成
			newObject.sprite = Sprite::Create();
			newObject.transform.scale = Vector3{ 256.0f, 256.0f, 1.0f }; //ピクセル単位なのでスケールをあらかじめ設定
			newObject.transform.translation = Vector3{ //座標がスプライトの中心なので、左上が原点になるよう位置を調整
				newObject.transform.scale.x / 2.0f,
				newObject.transform.scale.y / 2.0f,
				1.0f
			};
			break;

		case ObjectType::kPlane:
			//平面モデルを生成
			newObject.model = Model::CreateFromOBJ("plane.obj");
			break;

		case ObjectType::kSphere:
			//球モデルを生成
			newObject.model = Model::CreateSphere();
			break;
		}

		objects_.push_back(std::move(newObject));
	}

	ImGui::Spacing();

	//==================================================
	// 各オブジェクトの共通編集リスト
	//==================================================

	//削除予約インデックス
	int deleteIndex = -1;

	for (int i = 0; i < static_cast<int>(objects_.size()); ++i) {
		auto& obj = objects_[i];

		ImGui::PushID(obj.id);

		if (ImGui::CollapsingHeader("Object", ImGuiTreeNodeFlags_DefaultOpen)) {
			//トランスフォーム編集
			ImGui::DragFloat3("Translation", &obj.transform.translation.x, 0.01f);
			ImGui::DragFloat3("Rotation", &obj.transform.rotation.x, 0.01f);
			ImGui::DragFloat3("Scale", &obj.transform.scale.x, 0.01f);

			//削除
			if (ImGui::Button("Delete")) {
				deleteIndex = i;
			}

			//マテリアル編集
			if (ImGui::CollapsingHeader("Material")) {
				ImGui::Text("b");
			}
		}

		ImGui::PopID();
	}

	if (deleteIndex != -1) {
		objects_.erase(objects_.begin() + deleteIndex);
	}

	ImGui::Spacing();

	//==================================================
	// ライティング編集
	//==================================================

	if (ImGui::CollapsingHeader("Light")) {
		ImGui::Text("light");
	}

	ImGui::End();
#endif
}
