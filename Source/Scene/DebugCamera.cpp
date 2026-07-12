#include "Scene/DebugCamera.h"

#include "Input/Input.h"

#include <cmath>
#include <dinput.h>
#include <numbers>

DebugCamera::DebugCamera()
	:camera_(1280.0f, 720.0f) {
	yaw_ = 0.0f;
	pitch_ = 0.0f;
}

void DebugCamera::Update() {
	Rotate();
	Move();

	camera_.Update();
}

void DebugCamera::Rotate() {
	if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		yaw_ -= 0.01f;
	}

	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		yaw_ += 0.01f;
	}

	if (Input::GetInstance()->PushKey(DIK_UP)) {
		pitch_ -= 0.01f;
	}

	if (Input::GetInstance()->PushKey(DIK_DOWN)) {
		pitch_ += 0.01f;
	}

	camera_.SetRotation({ pitch_, yaw_, 0.0f });
}

void DebugCamera::Move() {
	//現在向いている方向を求める
	Vector3 forward = {
		std::cos(pitch_) * std::sin(yaw_),
		-std::sin(pitch_),
		std::cos(pitch_) * std::cos(yaw_)
	};

	forward = forward.Normalized();
	Vector3 worldUp = { 0.0f, 1.0f, 0.0f };
	Vector3 right = Cross(worldUp, forward).Normalized();
	Vector3 currentPos = camera_.GetTranslation();
	Vector3 moveDir = { 0.0f, 0.0f, 0.0f };
	float speed = 0.1f;

	//前後移動
	if (Input::GetInstance()->PushKey(DIK_W)) {
		moveDir += forward;
	}

	if (Input::GetInstance()->PushKey(DIK_S)) {
		moveDir -= forward;
	}

	//左右移動
	if (Input::GetInstance()->PushKey(DIK_A)) {
		moveDir -= right;
	}

	if (Input::GetInstance()->PushKey(DIK_D)) {
		moveDir += right;
	}

	//上下移動
	if (Input::GetInstance()->PushKey(DIK_E)) {
		moveDir += worldUp;
	}

	if (Input::GetInstance()->PushKey(DIK_Q)) {
		moveDir -= worldUp;
	}

	//座標の更新
	if (moveDir.Length() > 0.0f) {
		currentPos += moveDir.Normalized() * speed;
	}

	camera_.SetTranslation(currentPos);
}
