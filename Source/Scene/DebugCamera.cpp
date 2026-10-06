#include "Scene/DebugCamera.h"

#include "Input/Input.h"

#include <cmath>
#include <dinput.h>
#include <numbers>

DebugCamera::DebugCamera()
	:m_camera(1280.0f, 720.0f) {
	m_yaw = 0.0f;
	m_pitch = 0.0f;

	m_input = Input::GetInstance();
}

void DebugCamera::Update() {
	Rotate();
	Move();

	m_camera.Update();
}

void DebugCamera::Rotate() {
	if (m_input->GetMouse().Push(MouseButton::Right)) {
		m_yaw += static_cast<float>(m_input->GetMouse().GetMoveX()) * m_rotateSensitivity;
		m_pitch += static_cast<float>(m_input->GetMouse().GetMoveY()) * m_rotateSensitivity;
	}

	Vector3 cameraRotation = m_camera.GetRotation();

	cameraRotation.x = m_pitch;
	cameraRotation.y = m_yaw;

	m_camera.SetRotation({ m_pitch, m_yaw, 0.0f });
}

void DebugCamera::Move() {
	if (m_input->GetMouse().Push(MouseButton::Right)) {
		//現在向いている方向を求める
		Vector3 forward = {
			std::cos(m_pitch) * std::sin(m_yaw),
			-std::sin(m_pitch),
			std::cos(m_pitch) * std::cos(m_yaw)
		};

		forward = forward.Normalized();
		Vector3 worldUp = { 0.0f, 1.0f, 0.0f };
		Vector3 right = Cross(worldUp, forward).Normalized();
		Vector3 currentPos = m_camera.GetTranslation();
		Vector3 moveDir = { 0.0f, 0.0f, 0.0f };
		float speed = 0.1f;

		//前後移動
		if (m_input->GetKeyboard().Push(DIK_W)) {
			moveDir += forward;
		}

		if (m_input->GetKeyboard().Push(DIK_S)) {
			moveDir -= forward;
		}

		//左右移動
		if (m_input->GetKeyboard().Push(DIK_A)) {
			moveDir -= right;
		}

		if (m_input->GetKeyboard().Push(DIK_D)) {
			moveDir += right;
		}

		//上下移動
		if (m_input->GetKeyboard().Push(DIK_E)) {
			moveDir += worldUp;
		}

		if (m_input->GetKeyboard().Push(DIK_Q)) {
			moveDir -= worldUp;
		}

		//座標の更新
		if (moveDir.Length() > 0.0f) {
			currentPos += moveDir.Normalized() * speed;
		}

		m_camera.SetTranslation(currentPos);
	}
}
