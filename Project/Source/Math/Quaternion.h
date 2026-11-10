#pragma once

#include "Vector3.h"

#include <cmath>

/// <summary>
/// クォータニオン
/// </summary>
struct Quaternion {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

	//=========================================================
	// base operations
	//=========================================================

	//単位クォータニオン
	static Quaternion Identity() noexcept {
		return { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	//共役
	Quaternion Conjugate() const noexcept {
		return { -x, -y, -z, w };
	}

	//ノルムの2乗
	float LengthSquared() const noexcept {
		return (x * x) + (y * y) + (z * z) + (w * w);
	}

	float Length() const noexcept {
		return std::sqrt(LengthSquared());
	}

	//正規化(非破壊)
	Quaternion Normalized() const noexcept {
		if (float lengthSquared = LengthSquared(); lengthSquared > 0.0f) {
			float inverseLength = 1.0f / std::sqrt(lengthSquared);
			return { x * inverseLength, y * inverseLength, z * inverseLength, w * inverseLength };
		}

		return Identity();
	}

	//逆クォータニオン
	Quaternion Inversed() const noexcept {
		if (float length_squared = LengthSquared(); length_squared > 0.0f) {
			float inverse = 1.0f / length_squared;
			Quaternion c = Conjugate();
			return { c.x * inverse, c.y * inverse, c.z * inverse, c.w * inverse };
		}

		return Identity();
	}

	//=========================================================
	// operators
	//=========================================================

	/*--- 二項演算子 ---*/

	friend Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
		Vector3 q1_vector = { q1.x, q1.y, q1.z };
		Vector3 q2_vector = { q2.x, q2.y, q2.z };
		Vector3 result = Cross(q1_vector, q2_vector) + q2.w * q1_vector + q1.w * q2_vector;

		return {
			result.x,
			result.y,
			result.z,
			q1.w * q2.w - Dot(q1_vector, q2_vector)
		};
	}

	/*--- 比較演算子 ---*/

	bool operator==(const Quaternion& quaternion) const noexcept = default;

	//=========================================================
	// functions
	//=========================================================

	//軸生成
	static Quaternion MakeRotationAxisAngle(const Vector3& axis, float angle);

	//2つのベクトルを一致させるクォータニオン生成
	static Quaternion DirectionToDirection(const Vector3& from, const Vector3& to);
};

//=========================================================
// functions
//=========================================================

//ノルム
inline float Length(const Quaternion& quaternion) noexcept {
	return quaternion.Length();
}

inline Vector3 RotationVector(const Vector3& v, const Quaternion& q) {
	Quaternion result = { v.x, v.y, v.z, 0.0f };
	result = q * result * q.Conjugate();

	return Vector3{ result.x, result.y, result.z };
}
