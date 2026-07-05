#include <cmath>

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

Matrix4x4 Matrix4x4::Inversed() const {
	Matrix4x4 result{};

	//行列式
	float det =
		(m[0][0] * m[1][1] * m[2][2] * m[3][3]) + (m[0][0] * m[1][2] * m[2][3] * m[3][1]) + (m[0][0] * m[1][3] * m[2][1] * m[3][2]) -
		(m[0][0] * m[1][3] * m[2][2] * m[3][1]) - (m[0][0] * m[1][2] * m[2][1] * m[3][3]) - (m[0][0] * m[1][1] * m[2][3] * m[3][2]) -
		(m[0][1] * m[1][0] * m[2][2] * m[3][3]) - (m[0][2] * m[1][0] * m[2][3] * m[3][1]) - (m[0][3] * m[1][0] * m[2][1] * m[3][2]) +
		(m[0][3] * m[1][0] * m[2][2] * m[3][1]) + (m[0][2] * m[1][0] * m[2][1] * m[3][3]) + (m[0][1] * m[1][0] * m[2][3] * m[3][2]) +
		(m[0][1] * m[1][2] * m[2][0] * m[3][3]) + (m[0][2] * m[1][3] * m[2][0] * m[3][1]) + (m[0][3] * m[1][1] * m[2][0] * m[3][2]) -
		(m[0][3] * m[1][2] * m[2][0] * m[3][1]) - (m[0][2] * m[1][1] * m[2][0] * m[3][3]) - (m[0][1] * m[1][3] * m[2][0] * m[3][2]) -
		(m[0][1] * m[1][2] * m[2][3] * m[3][0]) - (m[0][2] * m[1][3] * m[2][1] * m[3][0]) - (m[0][3] * m[1][1] * m[2][2] * m[3][0]) +
		(m[0][3] * m[1][2] * m[2][1] * m[3][0]) + (m[0][2] * m[1][1] * m[2][3] * m[3][0]) + (m[0][1] * m[1][3] * m[2][2] * m[3][0]);

	float invDet = 1.0f / det;

	/*
	0行目
	--------------------*/

	//0列
	result.m[0][0] =
		m[1][1] * m[2][2] * m[3][3] +
		m[1][2] * m[2][3] * m[3][1] +
		m[1][3] * m[2][1] * m[3][2] -
		m[1][3] * m[2][2] * m[3][1] -
		m[1][2] * m[2][1] * m[3][3] -
		m[1][1] * m[2][3] * m[3][2];

	//1列
	result.m[0][1] =
		-m[0][1] * m[2][2] * m[3][3] -
		m[0][2] * m[2][3] * m[3][1] -
		m[0][3] * m[2][1] * m[3][2] +
		m[0][3] * m[2][2] * m[3][1] +
		m[0][2] * m[2][1] * m[3][3] +
		m[0][1] * m[2][3] * m[3][2];

	//2列
	result.m[0][2] =
		m[0][1] * m[1][2] * m[3][3] +
		m[0][2] * m[1][3] * m[3][1] +
		m[0][3] * m[1][1] * m[3][2] -
		m[0][3] * m[1][2] * m[3][1] -
		m[0][2] * m[1][1] * m[3][3] -
		m[0][1] * m[1][3] * m[3][2];

	//3列
	result.m[0][3] =
		-m[0][1] * m[1][2] * m[2][3] -
		m[0][2] * m[1][3] * m[2][1] -
		m[0][3] * m[1][1] * m[2][2] +
		m[0][3] * m[1][2] * m[2][1] +
		m[0][2] * m[1][1] * m[2][3] +
		m[0][1] * m[1][3] * m[2][2];

	/*
	1行目
	--------------------*/

	//0列
	result.m[1][0] =
		-m[1][0] * m[2][2] * m[3][3] -
		m[1][2] * m[2][3] * m[3][0] -
		m[1][3] * m[2][0] * m[3][2] +
		m[1][3] * m[2][2] * m[3][0] +
		m[1][2] * m[2][0] * m[3][3] +
		m[1][0] * m[2][3] * m[3][2];

	//1列
	result.m[1][1] =
		m[0][0] * m[2][2] * m[3][3] +
		m[0][2] * m[2][3] * m[3][0] +
		m[0][3] * m[2][0] * m[3][2] -
		m[0][3] * m[2][2] * m[3][0] -
		m[0][2] * m[2][0] * m[3][3] -
		m[0][0] * m[2][3] * m[3][2];

	//2列
	result.m[1][2] =
		-m[0][0] * m[1][2] * m[3][3] -
		m[0][2] * m[1][3] * m[3][0] -
		m[0][3] * m[1][0] * m[3][2] +
		m[0][3] * m[1][2] * m[3][0] +
		m[0][2] * m[1][0] * m[3][3] +
		m[0][0] * m[1][3] * m[3][2];

	//3列
	result.m[1][3] =
		m[0][0] * m[1][2] * m[2][3] +
		m[0][2] * m[1][3] * m[2][0] +
		m[0][3] * m[1][0] * m[2][2] -
		m[0][3] * m[1][2] * m[2][0] -
		m[0][2] * m[1][0] * m[2][3] -
		m[0][0] * m[1][3] * m[2][2];
	/*
	2行目
	--------------------*/

	//0列
	result.m[2][0] =
		m[1][0] * m[2][1] * m[3][3] +
		m[1][1] * m[2][3] * m[3][0] +
		m[1][3] * m[2][0] * m[3][1] -
		m[1][3] * m[2][1] * m[3][0] -
		m[1][1] * m[2][0] * m[3][3] -
		m[1][0] * m[2][3] * m[3][1];

	//1列
	result.m[2][1] =
		-m[0][0] * m[2][1] * m[3][3] -
		m[0][1] * m[2][3] * m[3][0] -
		m[0][3] * m[2][0] * m[3][1] +
		m[0][3] * m[2][1] * m[3][0] +
		m[0][1] * m[2][0] * m[3][3] +
		m[0][0] * m[2][3] * m[3][1];

	//2列
	result.m[2][2] =
		m[0][0] * m[1][1] * m[3][3] +
		m[0][1] * m[1][3] * m[3][0] +
		m[0][3] * m[1][0] * m[3][1] -
		m[0][3] * m[1][1] * m[3][0] -
		m[0][1] * m[1][0] * m[3][3] -
		m[0][0] * m[1][3] * m[3][1];

	//3列
	result.m[2][3] =
		-m[0][0] * m[1][1] * m[2][3] -
		m[0][1] * m[1][3] * m[2][0] -
		m[0][3] * m[1][0] * m[2][1] +
		m[0][3] * m[1][1] * m[2][0] +
		m[0][1] * m[1][0] * m[2][3] +
		m[0][0] * m[1][3] * m[2][1];

	/*
	3行目
	--------------------*/

	//0列
	result.m[3][0] =
		-m[1][0] * m[2][1] * m[3][2] -
		m[1][1] * m[2][2] * m[3][0] -
		m[1][2] * m[2][0] * m[3][1] +
		m[1][2] * m[2][1] * m[3][0] +
		m[1][1] * m[2][0] * m[3][2] +
		m[1][0] * m[2][2] * m[3][1];

	//1列
	result.m[3][1] =
		m[0][0] * m[2][1] * m[3][2] +
		m[0][1] * m[2][2] * m[3][0] +
		m[0][2] * m[2][0] * m[3][1] -
		m[0][2] * m[2][1] * m[3][0] -
		m[0][1] * m[2][0] * m[3][2] -
		m[0][0] * m[2][2] * m[3][1];

	//2列
	result.m[3][2] =
		-m[0][0] * m[1][1] * m[3][2] -
		m[0][1] * m[1][2] * m[3][0] -
		m[0][2] * m[1][0] * m[3][1] +
		m[0][2] * m[1][1] * m[3][0] +
		m[0][1] * m[1][0] * m[3][2] +
		m[0][0] * m[1][2] * m[3][1];

	//3列
	result.m[3][3] =
		m[0][0] * m[1][1] * m[2][2] +
		m[0][1] * m[1][2] * m[2][0] +
		m[0][2] * m[1][0] * m[2][1] -
		m[0][2] * m[1][1] * m[2][0] -
		m[0][1] * m[1][0] * m[2][2] -
		m[0][0] * m[1][2] * m[2][1];

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			//invDetを乗算
			result.m[row][column] *= invDet;
		}
	}

	return result;
}

Matrix4x4 Matrix4x4::MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;

	return result;
}

Matrix4x4 Matrix4x4::MakeRotateXMatrix(float radian) {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[1][1] = std::cosf(radian);
	result.m[1][2] = std::sinf(radian);
	result.m[2][1] = -std::sinf(radian);
	result.m[2][2] = std::cosf(radian);

	return result;
}

Matrix4x4 Matrix4x4::MakeRotateYMatrix(float radian) {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[0][0] = std::cosf(radian);
	result.m[0][2] = -std::sinf(radian);
	result.m[2][0] = std::sinf(radian);
	result.m[2][2] = std::cosf(radian);

	return result;
}

Matrix4x4 Matrix4x4::MakeRotateZMatrix(float radian) {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[0][0] = std::cosf(radian);
	result.m[0][1] = std::sinf(radian);
	result.m[1][0] = -std::sinf(radian);
	result.m[1][1] = std::cosf(radian);

	return result;
}

Matrix4x4 Matrix4x4::MakeRotateXYZMatrix(const Vector3& rotate) {
	return MakeRotateXMatrix(rotate.x) * MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z);
}

Matrix4x4 Matrix4x4::MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;

	return result;
}

Matrix4x4 Matrix4x4::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	return
		Matrix4x4::MakeScaleMatrix(scale) *
		Matrix4x4::MakeRotateXYZMatrix(rotate) *
		Matrix4x4::MakeTranslateMatrix(translate);
}

Matrix4x4 Matrix4x4::MakePerspectiveMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result{};

	float cot = 1.0f / std::tan(fovY * 0.5f);
	float inverseRange = 1.0f / (farClip - nearClip);

	result.m[0][0] = cot / aspectRatio;
	result.m[1][1] = cot;
	result.m[2][2] = farClip * inverseRange;
	result.m[2][3] = 1.0f;
	result.m[3][2] = -nearClip * farClip * inverseRange;
	result.m[3][3] = 0.0f;

	return result;
}

Matrix4x4 Matrix4x4::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result{};

	float inverseWidth = 1.0f / (right - left);
	float inverseHeight = 1.0f / (top - bottom);
	float inverseDepth = 1.0f / (farClip - nearClip);

	result.m[0][0] = 2.0f * inverseWidth;
	result.m[1][1] = 2.0f * inverseHeight;
	result.m[2][2] = inverseDepth;
	result.m[3][0] = -(right + left) * inverseWidth;
	result.m[3][1] = -(top + bottom) * inverseHeight;
	result.m[3][2] = -nearClip * inverseDepth;
	result.m[3][3] = 1.0f;

	return result;
}
