#pragma once

struct Vector3;

/**
 * \struct Matrix4x4
 */
struct Matrix4x4 {
	float m[4][4];

	/*--------基本操作--------*/

	/// @brief 
	/// @return 
	static Matrix4x4 Identity() {
		Matrix4x4 result{};

		result.m[0][0] = 1.0f;
		result.m[1][1] = 1.0f;
		result.m[2][2] = 1.0f;
		result.m[3][3] = 1.0f;

		return result;
	}

	//! @brief 
	//! @return 
	Matrix4x4 Transpose() const {
		Matrix4x4 result{};

		for (int row = 0; row < 4; ++row) {
			for (int column = 0; column < 4; ++column) {
				result.m[row][column] = m[column][row];
			}
		}

		return result;
	}

	//逆行列
	Matrix4x4 Inversed() const;

	/*--------Transform生成--------*/

	//拡縮行列
	static Matrix4x4 MakeScaleMatrix(const Vector3& scale);

	//X軸回転行列
	static Matrix4x4 MakeRotateXMatrix(float radian);

	//Y軸回転行列
	static Matrix4x4 MakeRotateYMatrix(float radian);

	//Z軸回転行列
	static Matrix4x4 MakeRotateZMatrix(float radian);

	//XYZ回転行列
	static Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotate);

	//平行移動行列
	static Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

	//アフィン変換行列
	static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	/*--------投影行列--------*/

	//! @brief 
	//! @param fovY 
	//! @param aspectRatio 
	//! @param nearClip 
	//! @param farClip 
	//! @return 
	static Matrix4x4 MakePerspectiveMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	//正射影行列
	static Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
};

/*--------基本演算子--------*/

inline Matrix4x4 operator+(const Matrix4x4 m1, const Matrix4x4& m2) {
	Matrix4x4 result{};

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			result.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}

	return result;
}

inline Matrix4x4 operator-(const Matrix4x4 m1, const Matrix4x4& m2) {
	Matrix4x4 result{};

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			result.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}

	return result;
}

inline Matrix4x4 operator*(const Matrix4x4 m1, const Matrix4x4& m2) {
	Matrix4x4 result{};

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			result.m[row][column] =
				m1.m[row][0] * m2.m[0][column] +
				m1.m[row][1] * m2.m[1][column] +
				m1.m[row][2] * m2.m[2][column] +
				m1.m[row][3] * m2.m[3][column];
		}
	}

	return result;
}
