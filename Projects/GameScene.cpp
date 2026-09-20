#include "GameScene.h"

#include "Graphics/GraphicsSystem.h"
#include "Audio/Audio.h"
#include "Input/Input.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

GameScene::~GameScene() {
	//サウンドデータを解放
	Audio::GetInstance()->SoundUnload(&soundFanfare_);
}

void GameScene::Initialize() {
	//--- カメラの初期化 ---

	//2Dカメラ
	camera2D_.Initialize(1280.0f, 720.0f);
	camera2D_.SetProjectionType(ProjectionType::Orthographic);

	//3Dカメラ
	camera3D_.Initialize(1280.0f, 720.0f);

	//デバッグカメラ
	debugCamera_.GetCamera().Initialize(1280.0f, 720.0f);

	//現在有効なカメラ
	activeCamera_ = camera3D_;

	//--- ライトの初期化 ---

	//平行ライト
	directionalLight_.Initialize();

	//--- 音声ファイルの読み込み ---

	//ファンファーレ
	soundFanfare_ = Audio::GetInstance()->SoundLoadWave("Resources/fanfare.wav");
}

void GameScene::Update() {
	//--- imgui操作 ---

	ControlImGui();

	//--- カメラの更新と切り替え ---

	camera2D_.Update();

	if (Input::GetInstance()->TriggerKey(DIK_F1)) {
		isValidDebugCamera_ = !isValidDebugCamera_;
	}

	if (isValidDebugCamera_) {
		activeCamera_ = debugCamera_.GetCamera();
		debugCamera_.Update();
	} else {
		activeCamera_ = camera3D_;
		camera3D_.Update();
	}

	//--- ライトの更新 ---

	directionalLight_.Update();
}

void GameScene::Draw() {
	//コマンドリストの取得
	auto commandList = GraphicsSystem::GetInstance()->GetCommandList();

	//ライトの設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLight_.GetGPUVirtualAddress());

	for (auto& obj : objects_) {
		if (obj.model) {
			obj.model->Draw(obj.transform, activeCamera_.GetViewProjMatrix());
		} else if (obj.sprite) {
			obj.sprite->Draw(obj.transform, camera2D_.GetViewProjMatrix());
		}
	}
}

void GameScene::ControlImGui() {
#ifdef _DEBUG
	ImGui::Begin("Settings");

	//オブジェクトの生成
	ImGuiCreateObject();

	//オブジェクトの編集
	ImGuiObjectEdit();

	//ライティング編集
	ImGuiLightEdit();

	//音声ファイルを再生
	if (ImGui::Button("Start Sound")) {
		//ファンファーレを流す
		Audio::GetInstance()->SoundPlayWave(soundFanfare_);
	}

	ImGui::End();
#endif
}

void GameScene::ImGuiCreateObject() {
	const char* objectTypeNames[] = { "Sprite", "Plane", "Sphere", "UtahTeapot", "StanfordBunny" };

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

		case ObjectType::kUtahTeapot:
			//ティーポットモデルを生成
			newObject.model = Model::CreateFromOBJ("teapot.obj");
			break;

		case ObjectType::kStanfordBunny:
			//うさぎモデルを生成
			newObject.model = Model::CreateFromOBJ("bunny.obj");
			break;
		}

		objects_.push_back(std::move(newObject));
	}
}

void GameScene::ImGuiObjectEdit() {
	//削除予約インデックス
	int deleteIndex = -1;

	for (int i = 0; i < static_cast<int>(objects_.size()); ++i) {
		auto& obj = objects_[i];

		ImGui::PushID(obj.id);

		if (ImGui::CollapsingHeader("Object", ImGuiTreeNodeFlags_DefaultOpen)) {
			float speed = 0.01f;
			if (obj.sprite) {
				speed = 0.1f;
			}
			//トランスフォーム編集
			ImGui::DragFloat3("Translation", &obj.transform.translation.x, speed);
			ImGui::DragFloat3("Rotation", &obj.transform.rotation.x, speed);
			ImGui::DragFloat3("Scale", &obj.transform.scale.x, speed);

			//削除
			if (ImGui::Button("Delete")) {
				deleteIndex = i;
			}

			ImGuiMaterialEdit(obj);
		}

		ImGui::PopID();
	}

	if (deleteIndex != -1) {
		objects_.erase(objects_.begin() + deleteIndex);
	}
}

void GameScene::ImGuiMaterialEdit(GameObject& obj) {
	//マテリアル編集
	if (!ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_Framed)) {
		return;
	}

	//UVトランスフォームとカラー(モデルとスプライトで異なるため)
	Transform uvTransform{};
	Vector4 color{};
	if (obj.model) {
		uvTransform = obj.model->GetUVTransform();
		color = obj.model->GetColor();
	} else if (obj.sprite) {
		uvTransform = obj.sprite->GetUVTransform();
		color = obj.sprite->GetColor();
	}

	//UVトランスフォーム
	ImGui::DragFloat2("UVTranslation", &uvTransform.translation.x, 0.01f);
	ImGui::DragFloat("UVRotation", &uvTransform.rotation.z, 0.01f);
	ImGui::DragFloat2("UVScale", &uvTransform.scale.x, 0.01f);

	//カラー
	ImGui::ColorEdit4("Color", &color.x);

	if (obj.model) {
		obj.model->SetUVTransform(uvTransform);
		obj.model->SetColor(color);
	} else if (obj.sprite) {
		obj.sprite->SetUVTransform(uvTransform);
		obj.sprite->SetColor(color);
	}

	//ライティング方式
	if (obj.model) {
		auto lightType = static_cast<int>(obj.model->GetLightType());
		const char* lightTypeNames[] = { "None", "Lambert", "HalfLambert" };
		ImGui::Combo("Light", &lightType, lightTypeNames, IM_ARRAYSIZE(lightTypeNames));
		obj.model->SetLightType(lightType);
	}

	ImGui::TreePop();
}

void GameScene::ImGuiLightEdit() {
	if (ImGui::CollapsingHeader("Light")) {
		//ライトカラー
		Vector4 color = directionalLight_.GetColor();
		ImGui::ColorEdit3("LightColor", &color.x);
		directionalLight_.SetColor(color);

		//ライトの向き
		Vector3 direction = directionalLight_.GetDirection();
		ImGui::SliderFloat3("LightDirection", &direction.x, -1.0f, 1.0f);
		directionalLight_.SetDirection(direction);

		//輝度
		float intensity = directionalLight_.GetIntensity();
		ImGui::DragFloat("intensity", &intensity, 0.01f, 0.0f, 10.0f);
		directionalLight_.SetIntensity(intensity);
	}
}
