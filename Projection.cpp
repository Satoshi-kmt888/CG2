#include <cmath>

#include "Projection.h"
#include "Matrix4x4.h"

Matrix4x4 PerspectiveProjection::GetMatrix() const {
	Matrix4x4 result{};

	result.m[0][0] = (1.0f / aspectRatio) * (1.0f / (tanf(fovY / 2.0f)));
	result.m[1][1] = 1.0f / (std::tanf(fovY / 2.0f));
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return result;
}

Matrix4x4 OrthographicProjection::GetMatrix() const {
	Matrix4x4 result{};

	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1.0f;

	return result;
}
