#include "Quaternion.h"

namespace {
	constexpr float k_tolerance = 1.0e-6f;
}

Quaternion Quaternion::MakeRotationAxisAngle(const Vector3& axis, float angle) {
	Quaternion quaternion = Quaternion::Identity();

	quaternion.w = std::cos(angle * 0.5f);

	quaternion.x = axis.x * std::sin(angle * 0.5f);
	quaternion.y = axis.y * std::sin(angle * 0.5f);
	quaternion.z = axis.z * std::sin(angle * 0.5f);

	return quaternion;
}

Quaternion Quaternion::DirectionToDirection(const Vector3& from, const Vector3& to) {
	float dot = Dot(from, to);

	//真逆の時を考慮する
	if (dot <= -1.0f + k_tolerance) {
		//fromに直交する軸を求める
		Vector3 orthogonal = (std::abs(from.x) < std::abs(from.z))
			? Vector3{ 1.0f, 0.0f, 0.0f } : Vector3{ 0.0f, 0.0f, 1.0f };
		Vector3 axis = Normalize(Cross(from, orthogonal));

		return Quaternion{ axis.x, axis.y, axis.z, 0.0f };
	}

	Vector3 cross = Cross(from, to);
	Quaternion rotation = {
		cross.x, cross.y, cross.z, 1.0f + dot
	};

	return rotation.Normalized();
}
