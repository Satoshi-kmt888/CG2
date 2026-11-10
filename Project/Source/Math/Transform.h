#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

/// <summary>
/// ワールド変換データ
/// </summary>
struct Transform {
	Vector3 scale = { 1.0f, 1.0f, 1.0f };
	Vector3 rotation = { 0.0f, 0.0f, 0.0f };
	Vector3 translation = { 0.0f, 0.0f, 0.0f };

	/// <summary>
	/// 合成されたアフィン変換行列を生成
	/// </summary>
	/// <returns>合成済みアフィン変換行列</returns>
	Matrix4x4 UpdateMatrix() const noexcept {
		return Matrix4x4::MakeAffine(scale, rotation, translation);
	}
};
