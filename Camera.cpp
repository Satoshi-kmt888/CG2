#include "Camera.h"

#include "WinApp.h"

void Camera::Initialize() {
	//ワールド変換データ
	rotation_ = { 0.0f, 0.0f, 0.0f };
	translation_ = { 0.0f, 0.0f, -5.0f };

	//透視投影
	fovY_ = 0.45f;
	aspectRatio_ =
		static_cast<float>(WinApp::GetInstance()->kClientWidth) /
		static_cast<float>(WinApp::GetInstance()->kClientHeight);

	//正射影
	left_ = 0.0f;
	top_ = 0.0f;
	right_ = static_cast<float>(WinApp::GetInstance()->kClientWidth);
	bottom_ = static_cast<float>(WinApp::GetInstance()->kClientHeight);

	//クリップ範囲
	nearClip_ = 0.1f;
	farClip_ = 100.0f;

	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = static_cast<float>(WinApp::GetInstance()->kClientWidth);
	viewport_.Height = static_cast<float>(WinApp::GetInstance()->kClientHeight);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.top = 0;
	scissorRect_.right = static_cast<long>(WinApp::GetInstance()->kClientWidth);
	scissorRect_.bottom = static_cast<long>(WinApp::GetInstance()->kClientHeight);

	//行列
	viewMatrix_ = Matrix4x4::Identity();
	viewPerspectiveProjectionMatrix_ = Matrix4x4::MakeProjectionFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);
	viewOrthographicProjectionMatrix_ = Matrix4x4::MakeOrthographicMatrix(left_, top_, right_, bottom_, nearClip_, farClip_);
}

void Camera::Update() {
	//行列を更新
	UpdateMatrix();
}

void Camera::UpdateMatrix() {
	//ワールド行列を作成
	Matrix4x4 worldMatrix = Matrix4x4::MakeRotateXYZMatrix(rotation_) * Matrix4x4::MakeTranslateMatrix(translation_);

	//ビュー行列を作成
	viewMatrix_ = worldMatrix.Inversed();

	//射影行列を作成
	perspectiveMatrix_ = Matrix4x4::MakeProjectionFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);
	orthographicMatrix_ = Matrix4x4::MakeOrthographicMatrix(left_, top_, right_, bottom_, nearClip_, farClip_);

	//ビュープロジェクション行列
	viewPerspectiveProjectionMatrix_ = viewMatrix_ * perspectiveMatrix_;
	viewOrthographicProjectionMatrix_= viewMatrix_* orthographicMatrix_;
}
