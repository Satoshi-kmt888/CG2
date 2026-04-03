#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"
#include "Projection.h"

/// <summary>
/// 投影の種類
/// </summary>
enum class ProjectionType {
	Perspective,
	Orthographic
};

/// <summary>
/// カメラ
/// </summary>
class Camera {
public:
	void Initialize();
	void Update();
	void Draw();

	Matrix4x4 GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
	Matrix4x4 GetViewportMatrix() const { return viewportMatrix_; }

private:
	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateMatrix();

private:
	//ワールド変換
	Vector3 rotation_;
	Vector3 translation_;

	//投影法
	ProjectionType projectionType_;
	PerspectiveProjection perspectiveProjection_;
	OrthographicProjection orthographicProjection_;

	//レンダリングパイプライン用の行列
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewProjectionMatrix_;
	Matrix4x4 viewportMatrix_;
	Matrix4x4 vpvMatrix_;
};
