#pragma once
#include <cmath>
#include <cassert>

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 {
	float x, y, z;

	/*--------複合代入演算子--------*/

	Vector3& operator+=(const Vector3& vector) {
		x += vector.x;
		y += vector.y;
		z += vector.z;

		return *this;
	}

	Vector3& operator-=(const Vector3& vector) {
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;

		return *this;
	}

	Vector3& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;

		return *this;
	}

	Vector3& operator/=(float scalar) {
		assert(scalar != 0.0f);
		float inverse = 1.0f / scalar;

		x *= inverse;
		y *= inverse;
		z *= inverse;

		return *this;
	}

	/*--------基本操作--------*/

	//ベクトルの長さを求める
	float Length() const {
		return std::sqrtf((x * x) + (y * y) + (z * z));
	}

	//正規化
	Vector3 Normalized() const {
		//ベクトルの長さを算出
		float vectorLength = std::sqrtf((x * x) + (y * y) + (z * z));

		if (vectorLength != 0.0f) {
			//各成分をベクトルの長さで割る
			return { x / vectorLength, y / vectorLength, z / vectorLength };
		}

		return { 0, 0, 0 };
	}
};

/*--------基本演算子--------*/

inline Vector3 operator+(Vector3 v1, const Vector3& v2) {
	v1 += v2;
	return v1;
}

inline Vector3 operator-(Vector3 v1, const Vector3& v2) {
	v1 -= v2;
	return v1;
}

inline Vector3 operator*(Vector3 v, float scalar) {
	v *= scalar;
	return v;
}

inline Vector3 operator*(float scalar, const Vector3& v) {
	return v * scalar;
}

/*--------ベクトル演算--------*/

//正規化
inline Vector3 Normalize(const Vector3& vector) {
	return vector.Normalized();
}

//内積
inline float Dot(const Vector3& v1, const Vector3& v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

//外積
inline Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = (v1.y * v2.z) - (v1.z * v2.y);
	result.y = (v1.z * v2.x) - (v1.x * v2.z);
	result.z = (v1.x * v2.y) - (v1.y * v2.x);

	return result;
}
