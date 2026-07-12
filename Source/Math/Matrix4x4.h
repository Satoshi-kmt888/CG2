#pragma once

/// <summary>
/// 4x4行列
/// </summary>
struct Matrix4x4 {
	float m[4][4];

	/*--------基本操作--------*/

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
			for (int column = 0; column < 4; ++column) {
				result.m[row][column] = m[column][row];
			}
		}

		return result;
	}

	/// <summary>
	/// 逆行列
	/// </summary>
	/// <returns></returns>
	Matrix4x4 Inversed() const noexcept;

	/*--------基本演算子--------*/

	friend Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) noexcept {
		Matrix4x4 result{};

		for (int row = 0; row < 4; ++row) {
			for (int column = 0; column < 4; ++column) {
				result.m[row][column] = m1.m[row][column] + m2.m[row][column];
			}
		}

		return result;
	}

	friend Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) noexcept {
		Matrix4x4 result{};

		for (int row = 0; row < 4; ++row) {
			for (int column = 0; column < 4; ++column) {
				result.m[row][column] = m1.m[row][column] - m2.m[row][column];
			}
		}

		return result;
	}

	friend Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) noexcept {
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
};
