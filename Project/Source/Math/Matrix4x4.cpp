#include "Matrix4x4.h"

#include <array>
#include <cmath>
#include <cstdlib>

namespace {
	/// <summary>
	/// 作業用配列4x8を初期化する
	/// </summary>
	/// <param name="matrix"></param>
	/// <returns></returns>
	/// 左側4x4: 逆行列を求めたい行列
	/// 右側4x4: 単位行列
	std::array<std::array<float, 8>, 4> MakeWorkMatrix(const Matrix4x4& matrix) {
		std::array<std::array<float, 8>, 4> work{};
		for (int row = 0; row < 4; ++row) {
			for (size_t col = 0; col < 4; ++col) {
				work[row][col] = matrix.m[row][col];
				work[row][col + 4] = (row == col) ? 1.0f : 0.0f;
			}
		}

		return work;
	}

	/// <summary>
	/// ピボット選択と入れ替えを行う
	/// </summary>
	/// <param name="work"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	bool PivotAndSwap(std::array<std::array<float, 8>, 4>& work, int index) {
		if (float pivot = work[index][index]; std::abs(pivot) >= 1e-8f) {
			return true;
		}

		int swap_row = -1;
		for (int row = 0; row < 4; ++row) {
			if (std::abs(work[row][index]) > 1e-8f) {
				swap_row = row;
				break;
			}
		}

		if (swap_row == -1) {
			return false; // 逆行列が存在しない
		}

		std::swap(work[index], work[swap_row]);
		return true;
	}
} // namespace

Matrix4x4 Matrix4x4::Inversed() const noexcept {
	//求める行列+単位行列の4x8行列を作成
	auto work = MakeWorkMatrix(*this);

	//ガウスジョルダン法による掃き出し
	for (int i = 0; i < 4; ++i) {
		if (!PivotAndSwap(work, i)) {
			return Matrix4x4::Identity();
		}

		float inv_pivot = 1.0f / work[i][i];

		//注目している行全体をピボットで割る
		for (int col = 0; col < 8; ++col) {
			work[i][col] *= inv_pivot;
		}

		//他の行の該当列を0にする
		for (int row = 0; row < 4; ++row) {
			if (row == i) {
				continue;
			}

			float factor = work[row][i];
			for (int col = 0; col < 8; ++col) {
				work[row][col] -= factor * work[i][col];
			}
		}
	}

	//計算した行列のうち、行列の右側部分が逆行列なので結果に格納する
	Matrix4x4 result{};
	for (int row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			result.m[row][col] = work[row][col + 4];
		}
	}

	return result;
}

Matrix4x4 Matrix4x4::MakeScale(const Vector3& scale) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;

	return result;
}

Matrix4x4 Matrix4x4::MakeRotationX(float radian) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
}

Matrix4x4 Matrix4x4::MakeRotationY(float radian) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -std::sin(radian);
	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
}

Matrix4x4 Matrix4x4::MakeRotationZ(float radian) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);

	return result;
}

Matrix4x4 Matrix4x4::MakeRotation(const Vector3& rotation) noexcept {
	return Matrix4x4::MakeRotationX(rotation.x) * Matrix4x4::MakeRotationY(rotation.y) * Matrix4x4::MakeRotationZ(rotation.z);
}

Matrix4x4 Matrix4x4::MakeRotation(const Quaternion& quaternion) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	float x = quaternion.x;
	float y = quaternion.y;
	float z = quaternion.z;
	float w = quaternion.w;

	result.m[0][0] = (w * w) + (x * x) - (y * y) - (z * z);
	result.m[0][1] = 2.0f * ((x * y) + (w * z));
	result.m[0][2] = 2.0f * ((x * z) - (w * y));

	result.m[1][0] = 2.0f * ((x * y) - (w * z));
	result.m[1][1] = (w * w) - (x * x) + (y * y) - (z * z);
	result.m[1][2] = 2.0f * ((y * z) + (w * x));

	result.m[2][0] = 2.0f * ((x * z) + (w * y));
	result.m[2][1] = 2.0f * ((y * z) - (w * x));
	result.m[2][2] = (w * w) - (x * x) - (y * y) + (z * z);

	return result;
}

Matrix4x4 Matrix4x4::MakeTranslation(const Vector3& translation) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[3][0] = translation.x;
	result.m[3][1] = translation.y;
	result.m[3][2] = translation.z;

	return result;
}

Matrix4x4 Matrix4x4::MakeAffine(const Vector3& scale, const Vector3& rotation, const Vector3& translation) noexcept {
	return
		Matrix4x4::MakeScale(scale) *
		Matrix4x4::MakeRotation(rotation) *
		Matrix4x4::MakeTranslation(translation);
}

Matrix4x4 Matrix4x4::MakePerspective(float fov_y, float aspect_ratio, float near_clip, float far_clip) noexcept {
	Matrix4x4 result{};

	float cot = 1.0f / std::tan(fov_y * 0.5f);
	float inverseRange = 1.0f / (far_clip - near_clip);

	result.m[0][0] = cot / aspect_ratio;
	result.m[1][1] = cot;
	result.m[2][2] = far_clip * inverseRange;
	result.m[2][3] = 1.0f;
	result.m[3][2] = -near_clip * far_clip * inverseRange;
	result.m[3][3] = 0.0f;

	return result;
}

Matrix4x4 Matrix4x4::MakeOrthographic(float left, float top, float right, float bottom, float near_clip, float far_clip) noexcept {
	Matrix4x4 result{};

	float inv_width = 1.0f / (right - left);
	float inv_height = 1.0f / (top - bottom);
	float inv_depth = 1.0f / (far_clip - near_clip);

	result.m[0][0] = 2.0f * inv_width;
	result.m[1][1] = 2.0f * inv_height;
	result.m[2][2] = inv_depth;
	result.m[3][0] = -(right + left) * inv_width;
	result.m[3][1] = -(top + bottom) * inv_height;
	result.m[3][2] = -near_clip * inv_depth;
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 Matrix4x4::MakeViewport(float width, float height, float min_depth, float max_depth, float left, float top) noexcept {
	Matrix4x4 result = Matrix4x4::Identity();

	float half_width = width * 0.5f;
	float half_height = height * 0.5f;

	result.m[0][0] = half_width;
	result.m[1][1] = -half_height;
	result.m[2][2] = max_depth - min_depth;
	result.m[3][0] = left + half_width;
	result.m[3][1] = top + half_height;
	result.m[3][2] = min_depth;

	return result;
}
