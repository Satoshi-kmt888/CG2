#pragma once

#include "Math/Transform.h"
#include "Scene/Camera.h"
#include "Scene/DirectionalLight.h"
#include "Scene/Model.h"
#include "Scene/Sprite.h"

#include <memory>

/// <summary>
/// モデル描画するシーン
/// </summary>
class GameScene {
public:
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

	//スプライト
	std::unique_ptr<Sprite> sprite_;
	Transform transformSprite_{};

	//平面モデル
	std::unique_ptr<Model> plane_;
	Transform transformPlane_{};

	//球モデル
	std::unique_ptr<Model> sphere_;
	Transform transformSphere_{};
};
