#pragma once

struct Vector3;
struct Quaternion;

/// <summary>
/// 4x4行列
/// </summary>
struct Matrix4x4 {
	float m[4][4];

	//==================================================
	// Base Operations
	//==================================================

	/// <summary>
	/// 単位行列
	/// </summary>
	/// <returns></returns>
	static Matrix4x4 Identity() noexcept {
		Matrix4x4 result{};

		result.m[0][0] = 1.0f;
		result.m[1][1] = 1.0f;
		result.m[2][2] = 1.0f;
		result.m[3][3] = 1.0f;

		return result;
	}

	/// <summary>
	/// 転置行列
	/// </summary>
	/// <returns></returns>
	Matrix4x4 Transpose() const noexcept {
		Matrix4x4 result{};

		for (int row = 0; row < 4; ++row) {
			for (int col = 0; col < 4; ++col) {
				result.m[row][col] = m[col][row];
			}
		}

		return result;
	}

	/// <summary>
	/// 逆行列
	/// </summary>
	/// <returns></returns>
	Matrix4x4 Inversed() const noexcept;

	//==================================================
	// Operators
	//==================================================

	friend Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) noexcept {
		Matrix4x4 result{};

		for (int row = 0; row < 4; ++row) {
			for (int col = 0; col < 4; ++col) {
				result.m[row][col] = m1.m[row][col] + m2.m[row][col];
			}
		}

		return result;
	}

	friend Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) noexcept {
		Matrix4x4 result{};

		for (int row = 0; row < 4; ++row) {
			for (int col = 0; col < 4; ++col) {
				result.m[row][col] = m1.m[row][col] - m2.m[row][col];
			}
		}

		return result;
	}

	friend Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) noexcept {
		Matrix4x4 result{};

		for (int row = 0; row < 4; ++row) {
			for (int col = 0; col < 4; ++col) {
				result.m[row][col] =
					m1.m[row][0] * m2.m[0][col] +
					m1.m[row][1] * m2.m[1][col] +
					m1.m[row][2] * m2.m[2][col] +
					m1.m[row][3] * m2.m[3][col];
			}
		}

		return result;
	}

	//======================================================
	// Functions
	//======================================================

	//拡縮行列を生成
	static Matrix4x4 MakeScale(const Vector3& scale) noexcept;

	static Matrix4x4 MakeRotationX(float radian) noexcept; //X軸回転行列を生成
	static Matrix4x4 MakeRotationY(float radian) noexcept; //Y軸回転行列を生成
	static Matrix4x4 MakeRotationZ(float radian) noexcept; //Z軸回転行列を生成
	static Matrix4x4 MakeRotation(const Vector3& rotation) noexcept; //回転行列を生成

	//クォータニオン回転行列を生成
	static Matrix4x4 MakeRotation(const Quaternion& quaternion) noexcept;

	//平行移動行列を生成
	static Matrix4x4 MakeTranslation(const Vector3& translation) noexcept;

	//アフィン変換行列を生成
	static Matrix4x4 MakeAffine(const Vector3& scale, const Vector3& rotation, const Vector3& translation) noexcept;

	//透視投影行列を生成
	static Matrix4x4 MakePerspective(float fov_y, float near_clip, float far_clip, float aspect_ratio) noexcept;
	//正射影行列を生成
	static Matrix4x4 MakeOrthographic(float width, float height, float near_clip, float far_clip) noexcept;
	//ビューポート行列を生成
	static Matrix4x4 MakeViewport(float width, float height, float min_depth, float max_depth, float left, float top) noexcept;
};
