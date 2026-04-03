#include <numbers>

#include "Camera.h"

void Camera::Initialize() {
	rotation_ = { 0.0f, 0.0f, 0.0f };
	translation_ = { 0.0f, 0.0f, 0.0f };

	projectionType_ = ProjectionType::Perspective;

	perspectiveProjection_.fovY = 45.0f * std::numbers::pi_v<float> / 180.0f;
	perspectiveProjection_.aspectRatio = static_cast<float>(1280) / static_cast<float>(720);
	perspectiveProjection_.nearClip = 0.1f;
	perspectiveProjection_.farClip = 100.0f;

	orthographicProjection_.left = -5.0f;
	orthographicProjection_.right = 5.0f;
	orthographicProjection_.top = 5.0f;
	orthographicProjection_.bottom = -5.0f;
	orthographicProjection_.nearClip = 0.1f;
	orthographicProjection_.farClip = 100.0f;
}

void Camera::Update() {
	//ビュープロジェクション行列の更新
	UpdateMatrix();
}

void Camera::Draw() {}

void Camera::UpdateMatrix() {
	//ワールド行列を作成
	worldMatrix_ = Matrix4x4::MakeRotateXYZMatrix(rotation_) * Matrix4x4::MakeTranslateMatrix(translation_);

	//ビュー行列を作成
	viewMatrix_ = worldMatrix_.Inversed();

	//射影行列を作成
	if (projectionType_ == ProjectionType::Perspective) {
		projectionMatrix_ = perspectiveProjection_.GetMatrix();
	} else if (projectionType_ == ProjectionType::Orthographic) {
		projectionMatrix_ = orthographicProjection_.GetMatrix();
	}

	//ビュープロジェクション行列
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}
