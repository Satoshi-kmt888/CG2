#pragma once

#include <cassert>
#include <cmath>

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 {
	float x = 0.0f;
	float y = 0.0f;

	//==================================================
	// base operations
	//==================================================

	//ノルムの2乗
	float LengthSquared() const noexcept {
		return (x * x) + (y * y);
	}

	//ノルム
	float Length() const noexcept {
		return std::sqrt(LengthSquared());
	}

	//正規化(非破壊)
	Vector2 Normalized() const noexcept {
		if (float length_squared = LengthSquared(); length_squared > 0.0f) {
			float length_inv = 1.0f / std::sqrt(length_squared);
			return { x * length_inv, y * length_inv };
		}

		return { 0.0f, 0.0f };
	}

	//==================================================
	// operators
	//==================================================

	/*--- 単項演算子 ---*/

	friend Vector2 operator-(const Vector2& vector) noexcept {
		return { -vector.x, -vector.y };
	}

	/*--- 二項演算子 ---*/

	friend Vector2 operator+(Vector2 v1, const Vector2& v2) noexcept {
		v1 += v2;
		return v1;
	}

	friend Vector2 operator-(Vector2 v1, const Vector2& v2) noexcept {
		v1 -= v2;
		return v1;
	}

	friend Vector2 operator*(Vector2 vector, float scalar) noexcept {
		vector *= scalar;
		return vector;
	}

	friend Vector2 operator*(float scalar, const Vector2& vector) noexcept {
		return vector * scalar;
	}

	friend Vector2 operator/(Vector2 vector, float scalar) noexcept {
		vector /= scalar;
		return vector;
	}

	/*--- 複合代入演算子 ---*/

	Vector2& operator+=(const Vector2& vector) noexcept {
		x += vector.x;
		y += vector.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& vector) noexcept {
		x -= vector.x;
		y -= vector.y;
		return *this;
	}

	Vector2& operator*=(float scalar) noexcept {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2& operator/=(float scalar) noexcept {
		assert(scalar != 0.0f);
		if (scalar == 0.0f) {
			x = 0.0f; y = 0.0f;
			return *this;
		}
		float inverse = 1.0f / scalar;

		x *= inverse;
		y *= inverse;

		return *this;
	}

	/*--- 比較演算子 ---*/

	bool operator==(const Vector2& vector) const noexcept = default;
};

//======================================================
// functions
//======================================================

inline float Length(const Vector2& vector) noexcept {
	return vector.Length();
}

inline Vector2 Normalize(const Vector2& vector) noexcept {
	return vector.Normalized();
}

inline float Dot(const Vector2& v1, const Vector2& v2) noexcept {
	return (v1.x * v2.x) + (v1.y * v2.y);
}

inline float Cross(const Vector2& v1, const Vector2& v2) noexcept {
	return (v1.x * v2.y) - (v1.y * v2.x);
}