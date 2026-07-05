#include "Camera/Camera.h"

Camera::Camera(float width, float height) {
	Initialize(width, height);
}

void Camera::Initialize(float width, float height) {
	width_ = width;
	height_ = height;

	rotation_ = { 0.0f, 0.0f, 0.0f };
	translation_ = { 0.0f, 0.0f, -10.0f };
	fovY_ = 45.0f * std::numbers::pi_v<float> / 180.0f;
	nearClip_ = 0.1f;
	farClip_ = 1000.0f;
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
	if (rotation_ != rotation) {
		rotation_ = rotation;
		isDirty_ = true;
	}
}

void Camera::SetTranslation(const Vector3& translation) {
	if (translation_ != translation) {
		translation_ = translation;
		isDirty_ = true;
	}
}

void Camera::SetFovY(float fovY) {
	fovY_ = fovY;
	isDirty_ = true;
}

void Camera::SetWidth(float width) {
	width_ = width;
	isDirty_ = true;
}

void Camera::SetHeight(float height) {
	height_ = height;
	isDirty_ = true;
}

void Camera::SetProjectionType(ProjectionType projectionType) {
	projectionType_ = projectionType;
	isDirty_ = true;
}
#pragma endregion

void Camera::UpdateMatrix() {
	//ビュー行列の作成
	Matrix4x4 worldMatrix = Matrix4x4::MakeRotateXYZMatrix(rotation_) * Matrix4x4::MakeTranslateMatrix(translation_);
	viewMatrix_ = worldMatrix.Inversed();

	//プロジェクション行列の計算
	if (projectionType_ == ProjectionType::Perspective) {
		//透視投影行列を計算
		projectionMatrix_ = Matrix4x4::MakePerspectiveMatrix(fovY_, width_ / height_, nearClip_, farClip_);
	} else {
		float halfWidth = width_ * 0.5f;
		float halfHeight = height_ * 0.5f;

		//正射影行列を計算
		projectionMatrix_ = Matrix4x4::MakeOrthographicMatrix(
			0.0f, 0.0f, width_, height_,
			nearClip_, farClip_
		);
	}

	//ビュー・プロジェクション行列を計算
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}
