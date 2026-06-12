#pragma once

#include "Matrix4x4.h"
#include "Vector3.h"

namespace Matrix4x4Factory {
	/*--- Transform生成 ---*/

	/// @brief 拡縮行列の作成
	/// @param scale スケール成分
	/// @return 
	constexpr Matrix4x4 MakeScaleMatrix(const Vector3& scale) noexcept {
		return Matrix4x4{
			scale.x, 0.0f,    0.0f,    0.0f,
			0.0f,    scale.y, 0.0f,    0.0f,
			0.0f,    0.0f,    scale.z, 0.0f,
			0.0f,    0.0f,    0.0f,    1.0f
		};
	}

	/// @brief X軸回転行列の作成
	/// @param radian X回転成分
	/// @return 
	constexpr Matrix4x4 MakeRotateXMatrix(float radian) noexcept {
		const float cos = std::cos(radian);
		const float sin = std::sin(radian);
		return Matrix4x4{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f,  cos,  sin, 0.0f,
			0.0f, -sin,  cos, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	/// @brief Y軸回転行列の作成
	/// @param radian Y回転成分
	/// @return 
	constexpr Matrix4x4 MakeRotateYMatrix(float radian) noexcept {
		const float cos = std::cos(radian);
		const float sin = std::sin(radian);
		return Matrix4x4{
			 cos, 0.0f, -sin, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			 sin, 0.0f,  cos, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	/// @brief Z軸回転行列の作成
	/// @param radian Z回転成分
	/// @return 
	constexpr Matrix4x4 MakeRotateZMatrix(float radian) noexcept {
		const float cos = std::cos(radian);
		const float sin = std::sin(radian);
		return Matrix4x4{
			 cos,  sin, 0.0f, 0.0f,
			-sin,  cos, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	/// @brief XYZ回転行列
	/// @param rotate 回転成分
	/// @return 
	constexpr Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotate) noexcept {
		return MakeRotateXMatrix(rotate.x) * MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z);
	}

	/// @brief 平行移動行列の作成
	/// @param translate 平行移動成分
	/// @return 
	constexpr Matrix4x4 MakeTranslateMatrix(const Vector3& translate) noexcept {
		return Matrix4x4{
			1.0f,        0.0f,        0.0f,        0.0f,
			0.0f,        1.0f,        0.0f,        0.0f,
			0.0f,        0.0f,        1.0f,        0.0f,
			translate.x, translate.y, translate.z, 1.0f
		};
	}

	/// @brief アフィン変換行列の作成
	/// @param scale スケール成分
	/// @param rotate 回転成分
	/// @param translate 平行移動成分
	/// @return 
	constexpr Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) noexcept {
		return MakeScaleMatrix(scale) * MakeRotateXYZMatrix(rotate) * MakeTranslateMatrix(translate);
	}

	/*--- 投影行列 ---*/

	/// @brief 透視投影行列の作成
	/// @param[in] fovY 視野角
	/// @param[in] aspectRatio アスペクト比
	/// @param[in] nearClip 近クリップ面
	/// @param[in] farClip 遠クリップ面
	/// @return 
	constexpr Matrix4x4 MakePerspectiveMatrix(float fovY, float aspectRatio, float nearClip, float farClip) noexcept {
		const float cot = 1.0f / std::tan(fovY * 0.5f);
		const float inverseRange = 1.0f / (farClip - nearClip);

		return Matrix4x4{
			cot / aspectRatio, 0.0f, 0.0f,                               0.0f,
			0.0f,              cot,  0.0f,                               0.0f,
			0.0f,              0.0f, farClip * inverseRange,             1.0f,
			0.0f,              0.0f, -nearClip * farClip * inverseRange, 0.0f
		};
	}

	/// @brief 正射影行列の作成
	/// @param[in] left 切り取る範囲の左端
	/// @param[in] top 切り取る範囲の上端
	/// @param[in] right 切り取る範囲の右端
	/// @param[in] bottom 切り取る範囲の下端
	/// @param[in] nearClip 近クリップ面
	/// @param[in] farClip 遠クリップ面
	/// @return 
	constexpr Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) noexcept {
		const float inverseWidth = 1.0f / (right - left);
		const float inverseHeight = 1.0f / (top - bottom);
		const float inverseDepth = 1.0f / (farClip - nearClip);

		return Matrix4x4{
			2.0f * inverseWidth,            0.0f,                           0.0f,                      0.0f,
			0.0f,                           2.0f * inverseHeight,           0.0f,                      0.0f,
			0.0f,                           0.0f,                           inverseDepth,              0.0f,
			-(right + left) * inverseWidth, -(top + bottom) * inverseHeight, -nearClip * inverseDepth, 1.0f
		};
	}
} //namespace Matrix4x4Factory