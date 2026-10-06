#pragma once

#include "Math/Vector3.h"
#include "Scene/Camera.h"

class Input;

/// <summary>
/// デバッグカメラ
/// /// </summary>
class DebugCamera {
public:
	//==================================================
	// public methods
	//==================================================

	DebugCamera();
	~DebugCamera() = default;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	Camera& GetCamera() { return m_camera; }

private:
	//==================================================
	// private methods
	//==================================================

	void Rotate();
	void Move();

	//==================================================
	// private variables
	//==================================================

	Input* m_input = nullptr;

	Camera m_camera;

	float m_zoomSpeed = 0.01f;
	float m_rotateSensitivity = 0.002f;
	float m_yaw;
	float m_pitch;
	Vector3 m_forward = {};
};
