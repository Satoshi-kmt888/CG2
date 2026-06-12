#pragma once

#include <array>
#include <cassert>

/// @brief 4x4行列
struct alignas(16) Matrix4x4 {
	std::array<float, 16> m;

	/*--- 2次元配列として扱う ---*/

	constexpr float operator()(size_t row, size_t col) const noexcept {
		assert(row < 4 && col < 4);
		return m[row * 4 + col];
	}

	constexpr float& operator()(size_t row, size_t col) noexcept {
		assert(row < 4 && col < 4);
		return m[row * 4 + col];
	}

	/*--- 二項演算 ---*/

	friend constexpr Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) noexcept {
		Matrix4x4 result{};
		for (size_t i = 0; i < 16; ++i) {
			result.m[i] = m1.m[i] + m2.m[i];
		}
		return result;
	}

	friend constexpr Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) noexcept {
		Matrix4x4 result{};
		for (size_t i = 0; i < 16; ++i) {
			result.m[i] = m1.m[i] - m2.m[i];
		}
		return result;
	}

	friend constexpr Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) noexcept {
		Matrix4x4 result{};

		result.m[0] = m1.m[0] * m2.m[0] + m1.m[1] * m2.m[4] + m1.m[2] * m2.m[8] + m1.m[3] * m2.m[12];
		result.m[1] = m1.m[0] * m2.m[1] + m1.m[1] * m2.m[5] + m1.m[2] * m2.m[9] + m1.m[3] * m2.m[13];
		result.m[2] = m1.m[0] * m2.m[2] + m1.m[1] * m2.m[6] + m1.m[2] * m2.m[10] + m1.m[3] * m2.m[14];
		result.m[3] = m1.m[0] * m2.m[3] + m1.m[1] * m2.m[7] + m1.m[2] * m2.m[11] + m1.m[3] * m2.m[15];

		result.m[4] = m1.m[4] * m2.m[0] + m1.m[5] * m2.m[4] + m1.m[6] * m2.m[8] + m1.m[7] * m2.m[12];
		result.m[5] = m1.m[4] * m2.m[1] + m1.m[5] * m2.m[5] + m1.m[6] * m2.m[9] + m1.m[7] * m2.m[13];
		result.m[6] = m1.m[4] * m2.m[2] + m1.m[5] * m2.m[6] + m1.m[6] * m2.m[10] + m1.m[7] * m2.m[14];
		result.m[7] = m1.m[4] * m2.m[3] + m1.m[5] * m2.m[7] + m1.m[6] * m2.m[11] + m1.m[7] * m2.m[15];

		result.m[8] = m1.m[8] * m2.m[0] + m1.m[9] * m2.m[4] + m1.m[10] * m2.m[8] + m1.m[11] * m2.m[12];
		result.m[9] = m1.m[8] * m2.m[1] + m1.m[9] * m2.m[5] + m1.m[10] * m2.m[9] + m1.m[11] * m2.m[13];
		result.m[10] = m1.m[8] * m2.m[2] + m1.m[9] * m2.m[6] + m1.m[10] * m2.m[10] + m1.m[11] * m2.m[14];
		result.m[11] = m1.m[8] * m2.m[3] + m1.m[9] * m2.m[7] + m1.m[10] * m2.m[11] + m1.m[11] * m2.m[15];

		result.m[12] = m1.m[12] * m2.m[0] + m1.m[13] * m2.m[4] + m1.m[14] * m2.m[8] + m1.m[15] * m2.m[12];
		result.m[13] = m1.m[12] * m2.m[1] + m1.m[13] * m2.m[5] + m1.m[14] * m2.m[9] + m1.m[15] * m2.m[13];
		result.m[14] = m1.m[12] * m2.m[2] + m1.m[13] * m2.m[6] + m1.m[14] * m2.m[10] + m1.m[15] * m2.m[14];
		result.m[15] = m1.m[12] * m2.m[3] + m1.m[13] * m2.m[7] + m1.m[14] * m2.m[11] + m1.m[15] * m2.m[15];

		return result;
	}

	/*--- 基本操作 ---*/

	/// @brief 
	/// @return 
	static constexpr Matrix4x4 Identity() noexcept {
		return Matrix4x4{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	/// @brief 転置行列
	/// @return 
	Matrix4x4 constexpr Transpose() const noexcept {
		return Matrix4x4{
			m[0], m[4], m[8],  m[12],
			m[1], m[5], m[9],  m[13],
			m[2], m[6], m[10], m[14],
			m[3], m[7], m[11], m[15]
		};
	}

	/// @brief 逆行列
	/// @return 
	Matrix4x4 Inversed() const noexcept;
};
