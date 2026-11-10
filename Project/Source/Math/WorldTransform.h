#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

/// <summary>
/// ワールド変換データ
/// </summary>
struct WorldTransform {
	Vector3 scale = { 1.0f, 1.0f, 1.0f }; //拡縮
	Vector3 rotation = { 0.0f, 0.0f, 0.0f }; //回転
	Vector3 translation = { 0.0f, 0.0f, 0.0f }; //平行移動

	//ワールド行列
	Matrix4x4 mat_world = Matrix4x4::Identity();
	//親のワールド変換ポインタ
	const WorldTransform* parent = nullptr;

	//ワールド行列を更新
	void UpdateMatrix() noexcept {
		mat_world = Matrix4x4::MakeAffine(scale, rotation, translation);

		if (parent) {
			mat_world *= parent->mat_world;
		}
	}
};
