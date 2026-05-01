#pragma once

#include "BaseObject.h"
#include "Vector3.h"

#include <cstdint>
#include <numbers>

struct Matrix4x4;

/// <summary>
/// 球
/// </summary>
class Sphere : public BaseObject{
public://--- ライフサイクル ---
	void Initialize(ID3D12DescriptorHeap* descriptorHeap);
	void Update(const Matrix4x4& viewProjectionMatrix);
	void Draw();

private://--- メンバ変数 ---
	const uint32_t kSubdivision = 16;
	const float kLonEvery = 2.0f * std::numbers::pi_v<float> / kSubdivision;
	const float kLatEvery = std::numbers::pi_v<float> / kSubdivision;

	uint32_t vertexCount = kSubdivision * kSubdivision * 6;
};
