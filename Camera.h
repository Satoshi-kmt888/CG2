#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

#include <d3d12.h>

/**
*  \class Camera
 * \brief 3Dカメラを管理するクラス
 */
class Camera {
public://--- ライフサイクル ---
	void Initialize();
	void Update();

private://--- 内部関数 ---
	void UpdateMatrix();

public://--- ゲッター ---
	const D3D12_VIEWPORT& GetViewport() const { return viewport_; }
	const D3D12_RECT& GetScissorRect() const { return scissorRect_; }

	Matrix4x4 GetVppMatrix() const { return viewPerspectiveProjectionMatrix_; }
	Matrix4x4 GetVopMatrix() const { return viewOrthographicProjectionMatrix_; }

private://--- メンバ変数 ---
	//ワールド変換データ
	Vector3 rotation_;
	Vector3 translation_;

	//透視投影
	float fovY_;
	float aspectRatio_;

	//正射影
	float left_;
	float top_;
	float right_;
	float bottom_;

	//クリップ範囲
	float nearClip_;
	float farClip_;

	//ビューポート・シザー矩形
	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	//行列
	Matrix4x4 viewMatrix_;
	Matrix4x4 perspectiveMatrix_;
	Matrix4x4 orthographicMatrix_;
	Matrix4x4 viewPerspectiveProjectionMatrix_;
	Matrix4x4 viewOrthographicProjectionMatrix_;
};
