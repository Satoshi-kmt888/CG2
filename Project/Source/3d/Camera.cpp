#include "Camera.h"

Camera::Camera(float width, float height) {
	Initialize(width, height);
}

void Camera::Initialize(float width, float height) {
	m_width = width;
	m_height = height;

	m_rotation = { 0.0f, 0.0f, 0.0f };
	m_translation = { 0.0f, 0.0f, -10.0f };
	m_fov_y = 45.0f * std::numbers::pi_v<float> / 180.0f;
	m_look_at = { 0.0f, 0.0f, 0.0f };
	m_width = 1280.0f;
	m_height = 720.0f;
	m_near_clip = 0.1f;
	m_far_clip = 1000.0f;
	m_projection_type = ProjectionType::PROJECTION_PERSPECTIVE;

	UpdateMatrix();
	m_is_dirty = false;
}

void Camera::Update() {
	if (m_is_dirty) {
		UpdateMatrix();
		m_is_dirty = false;
	}
}

#pragma region Setters
void Camera::SetRotation(const Vector3& rotation) {
	if (m_rotation != rotation) {
		m_rotation = rotation;
		m_is_dirty = true;
	}
}

void Camera::SetTranslation(const Vector3& translation) {
	if (m_translation != translation) {
		m_translation = translation;
		m_is_dirty = true;
	}
}

void Camera::SetFovY(float fov_y) {
	m_fov_y = fov_y;
	m_is_dirty = true;
}

void Camera::SetLookAt(const Vector3& look_at) {
	m_look_at = look_at;
	m_is_dirty = true;
}

void Camera::SetWidth(float width) {
	m_width = width;
	m_is_dirty = true;
}

void Camera::SetHeight(float height) {
	m_height = height;
	m_is_dirty = true;
}

void Camera::SetProjectionType(ProjectionType projectionType) {
	m_projection_type = projectionType;
	m_is_dirty = true;
}
void Camera::SetWorldMatrix(const Matrix4x4& mat_world) {
	m_mat_world = mat_world;
	m_is_dirty = true;
}
void Camera::SetViewProjectionMatrix(const Matrix4x4& mat_view_projection) {
	m_mat_view_projection = mat_view_projection;
	m_is_dirty = true;
}
#pragma endregion

void Camera::UpdateMatrix() {
	//ビュー行列の作成
	m_mat_world = Matrix4x4::MakeRotation(m_rotation) * Matrix4x4::MakeTranslation(m_translation);
	m_mat_view = m_mat_world.Inversed();

	//プロジェクション行列の計算
	if (m_projection_type == ProjectionType::PROJECTION_PERSPECTIVE) {
		//透視投影行列を計算
		m_mat_projection = Matrix4x4::MakePerspective(m_fov_y, GetAspectRatio(), m_near_clip, m_far_clip);
	} else {
		//正射影行列を計算
		float distance = (m_look_at - m_translation).Length();
		float height = 2.0f * distance * std::tan(m_fov_y * 0.5f);
		float width = height * GetAspectRatio();

		float left = -width * 0.5f;
		float top = height * 0.5f;
		float right = width * 0.5f;
		float bottom = -height * 0.5f;

		m_mat_projection = Matrix4x4::MakeOrthographic(left, top, right, bottom, m_near_clip, m_far_clip);
	}

	//ビュー・プロジェクション行列を計算
	m_mat_view_projection = m_mat_view * m_mat_projection;
}
