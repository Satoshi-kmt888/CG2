#include "Camera.h"

#include <numbers>

Camera::Camera()
	:rotation_({ 0.0f, 0.0f, 0.0f }),
	translation_({ 0.0f, 0.0f, 0.0f }),
	fovY_(std::numbers::pi_v<float> / 4.0f),
	aspectRatio_(16.0f / 9.0f),
	nearClip_(0.1f),
	farClip_(1000.0f),
	projectionType_(ProjectionType::Perspective)
{
	viewMatrix_ = Matrix4x4::Identity();
	projectionMatrix_ = Matrix4x4::Identity();
	viewProjMatrix_ = Matrix4x4::Identity();
}

void Camera::Initialize(int width, int height) {
	//ワールド変換データ
	rotation_ = { 0.0f, 0.0f, 0.0f };
	translation_ = { 0.0f, 0.0f, -5.0f };

	//透視投影
	aspectRatio_ = static_cast<float>(width) / static_cast<float>(height);

	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = static_cast<float>(width);
	viewport_.Height = static_cast<float>(height);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	//シザー矩形の設定
	scissorRect_.left = 0;
	scissorRect_.top = 0;
	scissorRect_.right = static_cast<long>(width);
	scissorRect_.bottom = static_cast<long>(height);

	UpdateMatrix();
}


void Camera::Update() {
	//行列を更新
	UpdateMatrix();
}

void Camera::UpdateMatrix() {
	//ビュー行列を作成
	Matrix4x4 worldMatrix = Matrix4x4::MakeRotateXYZMatrix(rotation_) * Matrix4x4::MakeTranslateMatrix(translation_);
	viewMatrix_ = worldMatrix.Inversed();

	//射影行列を作成
	if (projectionType_ == ProjectionType::Perspective) {
		//3D:透視投影
		projectionMatrix_ = Matrix4x4::MakeProjectionFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);
	} else {
		//2D:正射影
		projectionMatrix_ = Matrix4x4::MakeOrthographicMatrix(
			static_cast<float>(scissorRect_.left), static_cast<float>(scissorRect_.top),
			static_cast<float>(scissorRect_.right), static_cast<float>(scissorRect_.bottom),
			nearClip_, farClip_
		);
	}

	//ビュープロジェクション行列
	viewProjMatrix_ = viewMatrix_ * projectionMatrix_;
}
