#pragma once

#include <cassert>
#include <cmath>

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	//==================================================
	// base operations
	//==================================================

	//ノルムの2乗
	float LengthSquared() const noexcept {
		return (x * x) + (y * y) + (z * z);
	}

	//ノルム
	float Length() const noexcept {
		return std::sqrt(LengthSquared());
	}

	//正規化(非破壊)
	Vector3 Normalized() const noexcept {
		if (float length_squared = LengthSquared(); length_squared > 0.0f) {
			float length_inv = 1.0f / std::sqrt(length_squared);
			return { x * length_inv, y * length_inv, z * length_inv };
		}

		return { 0.0f, 0.0f, 0.0f };
	}

	//==================================================
	// operators
	//==================================================

	/*--- 単項演算子 ---*/

	friend Vector3 operator-(const Vector3& v) noexcept {
		return { -v.x, -v.y, -v.z };
	}

	/*--- 二項演算子 ---*/

	friend Vector3 operator+(Vector3 v1, const Vector3& v2) noexcept {
		v1 += v2;
		return v1;
	}

	friend Vector3 operator-(Vector3 v1, const Vector3& v2) noexcept {
		v1 -= v2;
		return v1;
	}

	friend Vector3 operator*(Vector3 v, float scalar) noexcept {
		v *= scalar;
		return v;
	}

	friend Vector3 operator*(float scalar, const Vector3& v) noexcept {
		return v * scalar;
	}

	friend Vector3 operator/(Vector3 v, float s) noexcept {
		v /= s;
		return v;
	}

	/*--- 複合代入演算子 ---*/

	Vector3& operator+=(const Vector3& vector) noexcept {
		x += vector.x;
		y += vector.y;
		z += vector.z;
		return *this;
	}

	Vector3& operator-=(const Vector3& vector) noexcept {
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		return *this;
	}

	Vector3& operator*=(float scalar) noexcept {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	Vector3& operator/=(float scalar) noexcept {
		assert(scalar != 0.0f);
		if (scalar == 0.0f) {
			x = 0.0f; y = 0.0f; z = 0.0f;
			return *this;
		}
		float inverse = 1.0f / scalar;

		x *= inverse;
		y *= inverse;
		z *= inverse;

		return *this;
	}

	/*--- 比較演算子 ---*/

	bool operator==(const Vector3& vector) const noexcept = default;
};

//======================================================
// functions
//======================================================

//ノルム
inline float Length(const Vector3& vector) noexcept {
	return vector.Length();
}

//正規化
inline Vector3 Normalize(const Vector3& vector) noexcept {
	return vector.Normalized();
}

//内積
inline float Dot(const Vector3& v1, const Vector3& v2) noexcept {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

//外積
inline Vector3 Cross(const Vector3& v1, const Vector3& v2) noexcept {
	return Vector3{
		(v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x)
	};
}
