#include "Camera.h"

#include <cassert>

Camera::Camera(float width, float height) {
	Initialize(width, height);
}

void Camera::Initialize(float width, float height) {
	m_width = width;
	m_height = height;

	m_rotation = { 0.0f, 0.0f, 0.0f };
	m_translation = { 0.0f, 0.0f, -10.0f };
	m_fovY = 45.0f * std::numbers::pi_v<float> / 180.0f;
	m_near_clip = 0.1f;
	m_far_clip = 1000.0f;
	projectionType_ = ProjectionType::Perspective;

	UpdateMatrix();
	isDirty_ = false;
}

void Camera::Update() {
	if (isDirty_) {
		UpdateMatrix();
		isDirty_ = false;
	}
}

#pragma region Setters
void Camera::SetRotation(const Vector3& rotation) {
	if (m_rotation != rotation) {
		m_rotation = rotation;
		isDirty_ = true;
	}
}

void Camera::SetTranslation(const Vector3& translation) {
	if (m_translation != translation) {
		m_translation = translation;
		isDirty_ = true;
	}
}

void Camera::SetFovY(float fovY) {
	m_fovY = fovY;
	isDirty_ = true;
}

void Camera::SetWidth(float width) {
	m_width = width;
	isDirty_ = true;
}

void Camera::SetHeight(float height) {
	m_height = height;
	isDirty_ = true;
}

void Camera::SetProjectionType(ProjectionType projectionType) {
	projectionType_ = projectionType;
	isDirty_ = true;
}
#pragma endregion

void Camera::UpdateMatrix() {
	//ビュー行列の作成
	m_mat_world = Matrix4x4::MakeRotation(m_rotation) * Matrix4x4::MakeTranslation(m_translation);
	m_mat_view = m_mat_world.Inversed();

	//プロジェクション行列の計算
	if (projectionType_ == ProjectionType::Perspective) {
		//透視投影行列を計算
		m_mat_projection = Matrix4x4::MakePerspective(m_fovY, m_near_clip, m_far_clip, GetAspectRatio());
	} else {
		//正射影行列を計算
		m_mat_projection = Matrix4x4::MakeOrthographic(m_width, m_height, m_near_clip, m_far_clip);
	}

	//ビュー・プロジェクション行列を計算
	m_mat_view_projection = m_mat_view * m_mat_projection;
}
