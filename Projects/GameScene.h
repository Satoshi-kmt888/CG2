#pragma once

#include "Math/Transform.h"
#include "Scene/Camera.h"
#include "Scene/DirectionalLight.h"
#include "Scene/Model.h"
#include "Scene/Sprite.h"

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
	~GameScene() = default;

	//--- 公開関数 ---

	void Initialize();

	void Update();

	void Draw();

private:
	//--- 内部関数 ---

	void ControlImGui();

	//--- 内部変数 ---

	//カメラ
	Camera camera2D_{ 1280.0f, 720.0f };
	Camera camera3D_{ 1280.0f, 720.0f };

	//ライト
	DirectionalLight directionalLight_;

	std::vector<GameObject> objects_;
	uint32_t nextObjectID_ = 1;
	int selectedObjectTypeIndex_ = 0;
};
