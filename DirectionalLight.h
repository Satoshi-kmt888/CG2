#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "Vector3.h"
#include "Vector4.h"

struct ConstBufferData {
	Vector3 direction; //<! ライトの向き
	Vector4 color;     //<! ライトの色
	float intensity;   //<! 輝度
};

class DirectionalLight {
public:
	DirectionalLight() = default;

	void Initialize();
	void Update();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return resource_->GetGPUVirtualAddress();
	}

private:
	Vector3 direction_ = { 0.0f, -1.0f, 0.0f };
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	float intensity_ = 1.0f;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
	ConstBufferData* constBufferData_ = nullptr;
};
