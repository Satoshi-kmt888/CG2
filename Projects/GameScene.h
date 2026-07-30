#pragma once

#include "Math/Transform.h"
#include "Scene/Camera.h"
#include "Scene/DirectionalLight.h"
#include "Scene/Model.h"
#include "Scene/Sprite.h"
#include "Audio/Audio.h"

#include <memory>
#include <vector>

/// <summary>
/// モデル描画するシーン
/// </summary>
class GameScene {
public:
	//--- インナークラス ---

	//モデルの種類
	enum class ObjectType {
		kSprite,
		kPlane,
		kSphere,
		kUtahTeapot,
		kStanfordBunny
	};

	struct GameObject {
		uint32_t id = 0;
		std::string name;
		ObjectType type;
		std::unique_ptr<Sprite> sprite;
		std::unique_ptr<Model> model;
		Transform transform{};
	};

	//--- インスタンス管理 ---

	GameScene() = default;
	~GameScene();

	//--- 公開関数 ---

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	//--- 内部関数 ---

	/// <summary>
	/// imguiの操作
	/// </summary>
	void ControlImGui();

	/// <summary>
	/// imguiでオブジェクトを生成する
	/// </summary>
	void ImGuiCreateObject();

	/// <summary>
	/// imguiで生成したオブジェクトを編集する
	/// </summary>
	void ImGuiObjectEdit();

	/// <summary>
	/// imguiで生成したオブジェクトのマテリアルを編集する
	/// </summary>
	/// <param name="obj"></param>
	void ImGuiMaterialEdit(GameObject& obj);

	/// <summary>
	/// imguiでライトを編集する
	/// </summary>
	void ImGuiLightEdit();

	//--- 内部変数 ---

	//カメラ
	Camera camera2D_{ 1280.0f, 720.0f };
	Camera camera3D_{ 1280.0f, 720.0f };

	//ライト
	DirectionalLight directionalLight_;

	//オブジェクトコンテナ
	std::vector<GameObject> objects_;
	uint32_t nextObjectID_ = 1;
	int selectedObjectTypeIndex_ = 0;

	//音声データ
	Audio::SoundData soundFanfare_;
};
