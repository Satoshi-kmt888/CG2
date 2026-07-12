#pragma once

#include "Scene/Camera.h"
#include "Math/Vector3.h"

/// <summary>
/// デバッグカメラ
/// /// </summary>
class DebugCamera {
public:
	//--- コンストラクタ・デストラクタ ---

	DebugCamera();
	~DebugCamera() = default;

	//--- 公開関数 ---

	void Update();

	//--- ゲッター ---

	Camera& GetCamera() { return camera_; }

private:
	//--- 内部関数 ---

	void Rotate();

	void Move();

	//--- メンバ変数 ---

	Camera camera_;

	float zoomSpeed_ = 0.01f;
	float rotateSensitivity_ = 0.005f;
	float yaw_;
	float pitch_;
	Vector3 forward_ = { 0.0f, 0.0f, 0.0f };
};
