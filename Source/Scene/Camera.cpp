#include "Scene/Camera.h"

#include "Math/Transform.h"

#include <cassert>

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
	Matrix4x4 worldMatrix = Transform::MakeRotateXYZMatrix(rotation_) * Transform::MakeTranslateMatrix(translation_);
	viewMatrix_ = worldMatrix.Inversed();

	//プロジェクション行列の計算
	if (projectionType_ == ProjectionType::Perspective) {
		//透視投影行列を計算
		projectionMatrix_ = MakePerspectiveMatrix();
	} else {
		//正射影行列を計算
		projectionMatrix_ = MakeOrthographicMatrix();
	}

	//ビュー・プロジェクション行列を計算
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

Matrix4x4 Camera::MakePerspectiveMatrix() const noexcept {
	Matrix4x4 result{};

	float cot = 1.0f / std::tan(fovY_ * 0.5f);
	float inverseRange = 1.0f / (farClip_ - nearClip_);

	result.m[0][0] = cot / GetAspectRatio();
	result.m[1][1] = cot;
	result.m[2][2] = farClip_ * inverseRange;
	result.m[2][3] = 1.0f;
	result.m[3][2] = -nearClip_ * farClip_ * inverseRange;
	result.m[3][3] = 0.0f;

	return result;
}

Matrix4x4 Camera::MakeOrthographicMatrix() const noexcept {
	assert(width_ != 0.0f);
	assert(height_ != 0.0f);
	assert(farClip_ != nearClip_);

	Matrix4x4 result{};

	float left = 0.0f;
	float right = width_;
	float top = height_;
	float bottom = 0.0f;

	float inverseWidth = 1.0f / (right - left);
	float inverseHeight = 1.0f / (top - bottom);
	float inverseDepth = 1.0f / (farClip_ - nearClip_);

	result.m[0][0] = 2.0f * inverseWidth;
	result.m[1][1] = 2.0f * inverseHeight;
	result.m[2][2] = inverseDepth;
	result.m[3][0] = -(right + left) * inverseWidth;
	result.m[3][1] = -(top + bottom) * inverseHeight;
	result.m[3][2] = -nearClip_ * inverseDepth;
	result.m[3][3] = 1.0f;

	return result;
}
