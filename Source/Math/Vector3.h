#pragma once

#include <cassert>
#include <cmath>

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 {
	float x;
	float y;
	float z;

	/*--- 基本操作 ---*/

	/// <summary>
	/// ベクトルの長さ(ノルム)を計算
	/// </summary>
	/// <returns>ベクトルの長さ</returns>
	float Length() const noexcept {
		return std::sqrtf((x * x) + (y * y) + (z * z));
	}

	/// <summary>
	/// ベクトルの長さの2乗を計算
	/// </summary>
	/// <returns>ベクトルの長さの2乗</returns>
	float LengthSquared() const noexcept {
		return (x * x) + (y * y) + (z * z);
	}

	/// <summary>
	/// 正規化(非破壊)
	/// </summary>
	/// <returns>正規化されたベクトル</returns>
	Vector3 Normalized() const noexcept {
		if (float lengthSquared = (x * x) + (y * y) + (z * z); lengthSquared > 0.0f) {
			float inverseLength = 1.0f / std::sqrtf(lengthSquared);
			return { x * inverseLength, y * inverseLength, z * inverseLength };
		}

		return { 0.0f, 0.0f, 0.0f };
	}

	/*--- 単項演算子 ---*/

	friend Vector3 operator-(const Vector3& v) noexcept {
		return { -v.x, -v.y, -v.z };
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

	/*--- 比較演算子 ---*/

	bool operator==(const Vector3& vector) const noexcept = default;
};

/*--- ベクトル演算 ---*/

/// <summary>
/// 指定されたベクトルの長さを計算
/// </summary>
/// <param name="v">対象のベクトル</param>
/// <returns>ベクトルの長さ</returns>
inline float Length(const Vector3& v) noexcept {
	return v.Length();
}

/// <summary>
/// 正規化
/// </summary>
/// <param name="vector">対象のベクトル</param>
/// <returns>正規化されたベクトル</returns>
inline Vector3 Normalize(const Vector3& vector) noexcept {
	return vector.Normalized();
}

/// <summary>
/// 内積
/// </summary>
/// <param name="v1">ベクトル1</param>
/// <param name="v2">ベクトル2</param>
/// <returns>内積の結果</returns>
inline float Dot(const Vector3& v1, const Vector3& v2) noexcept {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

/// <summary>
/// 外積
/// </summary>
/// <param name="v1">ベクトル1</param>
/// <param name="v2">ベクトル2</param>
/// <returns>外積の結果</returns>
inline Vector3 Cross(const Vector3& v1, const Vector3& v2) noexcept {
	return Vector3{
		(v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x)
	};
}
