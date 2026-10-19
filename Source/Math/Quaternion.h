#pragma once

#include "Math/Vector3.h"

#include <cmath>

/// <summary>
/// クォータニオン
/// </summary>
struct Quaternion {
	float x;
	float y;
	float z;
	float w;

	static Quaternion Identity() noexcept {
		return { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	Quaternion Conjugate() const noexcept {
		return { -x, -y, -z, w };
	}

	Quaternion Inversed() const noexcept {
		if (float length_squared = LengthSquared(); length_squared > 0.0f) {
			float inverse = 1.0f / length_squared;
			Quaternion c = Conjugate();
			return { c.x * inverse, c.y * inverse, c.z * inverse, c.w * inverse };
		}

		return Identity();
	}

	float LengthSquared() const noexcept {
		return (x * x) + (y * y) + (z * z) + (w * w);
	}

	float Length() const noexcept {
		return std::sqrt(LengthSquared());
	}

	Quaternion Normalized() const noexcept {
		if (float lengthSquared = LengthSquared(); lengthSquared > 0.0f) {
			float inverseLength = 1.0f / std::sqrt(lengthSquared);
			return { x * inverseLength, y * inverseLength, z * inverseLength, w * inverseLength };
		}

		return { 0.0f, 0.0f, 0.0f, 1.0f };
	}

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
};
