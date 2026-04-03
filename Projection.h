#pragma once

struct Matrix4x4;

/// <summary>
/// 透視投影
/// </summary>
struct PerspectiveProjection {
	float fovY;
	float aspectRatio;
	float nearClip;
	float farClip;

	//透視投影法行列を取得
	Matrix4x4 GetMatrix() const;
};

/// <summary>
/// 正射影
/// </summary>
struct OrthographicProjection {
	float left;
	float top;
	float right;
	float bottom;
	float nearClip;
	float farClip;

	//正射影行列を取得
	Matrix4x4 GetMatrix() const;
};
