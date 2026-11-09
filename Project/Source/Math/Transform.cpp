#include "Math/Transform.h"

Matrix4x4 Transform::MakeScaleMatrix(const Vector3& scale) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;

	return result;
}

Matrix4x4 Transform::MakeRotateXMatrix(float radian) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[1][1] = std::cosf(radian);
	result.m[1][2] = std::sinf(radian);
	result.m[2][1] = -std::sinf(radian);
	result.m[2][2] = std::cosf(radian);

	return result;
}

Matrix4x4 Transform::MakeRotateYMatrix(float radian) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[0][0] = std::cosf(radian);
	result.m[0][2] = -std::sinf(radian);
	result.m[2][0] = std::sinf(radian);
	result.m[2][2] = std::cosf(radian);

	return result;
}

Matrix4x4 Transform::MakeRotateZMatrix(float radian) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[0][0] = std::cosf(radian);
	result.m[0][1] = std::sinf(radian);
	result.m[1][0] = -std::sinf(radian);
	result.m[1][1] = std::cosf(radian);

	return result;
}

Matrix4x4 Transform::MakeRotateXYZMatrix(const Vector3& rotation) noexcept {
	return MakeRotateXMatrix(rotation.x) * MakeRotateYMatrix(rotation.y) * MakeRotateZMatrix(rotation.z);
}

Matrix4x4 Transform::MakeTranslateMatrix(const Vector3& translation) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[3][0] = translation.x;
	result.m[3][1] = translation.y;
	result.m[3][2] = translation.z;

	return result;
}

Matrix4x4 Transform::MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation) noexcept {
	return
		Transform::MakeScaleMatrix(scale) *
		Transform::MakeRotateXYZMatrix(rotation) *
		Transform::MakeTranslateMatrix(translation);
}
