#pragma once

#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"

/// <summary>
/// ワールド変換データ
/// </summary>
struct Transform {
	Vector3 scale = { 1.0f, 1.0f, 1.0f };
	Vector3 rotation = { 0.0f, 0.0f, 0.0f };
	Vector3 translation = { 0.0f, 0.0f, 0.0f };

	//--- 基本操作 ---

	/// <summary>
	/// 合成されたアフィン変換行列を生成
	/// </summary>
	/// <returns>合成済みアフィン変換行列</returns>
	Matrix4x4 UpdateMatrix() const noexcept {
		return MakeAffineMatrix(scale, rotation, translation);
	}

	//--- 静的メンバ関数 ---

	/// <summary>
	/// 拡縮行列を計算
	/// </summary>
	/// <param name="scale">各軸の拡縮率</param>
	/// <returns>拡縮行列</returns>
	static Matrix4x4 MakeScaleMatrix(const Vector3& scale) noexcept;

	/// <summary>
	/// X軸回転行列を計算
	/// </summary>
	/// <param name="radian">回転角(ラジアン)</param>
	/// <returns>X軸回転行列</returns>
	static Matrix4x4 MakeRotateXMatrix(float radian) noexcept;

	/// <summary>
	/// Y軸回転行列を計算
	/// </summary>
	/// <param name="radian">回転角(ラジアン)</param>
	/// <returns>X軸回転行列</returns>
	static Matrix4x4 MakeRotateYMatrix(float radian) noexcept;

	/// <summary>
	/// Z軸回転行列を計算
	/// </summary>
	/// <param name="radian">回転角(ラジアン)</param>
	/// <returns>X軸回転行列</returns>
	static Matrix4x4 MakeRotateZMatrix(float radian) noexcept;

	/// <summary>
	/// XYZの合成回転行列を計算
	/// </summary>
	/// <param name="rotation">各軸の回転角(ラジアン)</param>
	/// <returns>合成された回転行列</returns>
	static Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotation) noexcept;

	/// <summary>
	/// 平行移動行列を計算
	/// </summary>
	/// <param name="translation">各軸の移動量</param>
	/// <returns>平行移動行列</returns>
	static Matrix4x4 MakeTranslateMatrix(const Vector3& translation) noexcept;

	/// <summary>
	/// アフィン変換行列を計算
	/// </summary>
	/// <param name="scale">拡縮率</param>
	/// <param name="rotation">回転角(ラジアン)</param>
	/// <param name="translation">移動量</param>
	/// <returns>アフィン変換行列</returns>
	static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation) noexcept;
};
