#include "Render/Material.h"

#include "Graphics/D3D12Utility.h"
#include "Graphics/TextureManager.h"
#include "Logging/Logger.h"

void Material::CreateBuffer(ID3D12Device* device) {
	if (!device) {
		LOG_ERROR("Material::CreateBufer 引数deviceがnullptrです。");
		return;
	}

	surfaceBuffer_ = D3D12Utility::CreateBufferResource(device, sizeof(SurfaceData));
	if (!surfaceBuffer_) {
		LOG_ERROR("Material::CreateBuffer メンバ変数surfaceBuffer_を生成することができませんでした。");
		return;
	}

	void* materialPtr = nullptr;
	HRESULT hr = surfaceBuffer_->Map(0, nullptr, &materialPtr);
	if (FAILED(hr)) {
		LOG_ERROR("Material::CreateBuffer メンバ変数surfaceBufferをマッピングすることができませんでした。");
		return;
	}

	surfaceData_ = static_cast<SurfaceData*>(materialPtr);
	if (surfaceData_) {
		*surfaceData_ = SurfaceData();
	}
}

void Material::Update() {
	if (!surfaceBuffer_) {
		return;
	}

	//uv座標変換データの計算
	Matrix4x4 uvTransformMatrix = Transform::MakeScaleMatrix(transform_.scale);
	uvTransformMatrix = uvTransformMatrix * Transform::MakeRotateZMatrix(transform_.rotation.z);
	uvTransformMatrix = uvTransformMatrix * Transform::MakeTranslateMatrix(transform_.translation);
	surfaceData_->uvTransform = uvTransformMatrix;
}

void Material::SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial, UINT rootParamIndexTexture) const {
	if (!commandList) {
		LOG_ERROR("Material::SetGraphicsCommand 引数commandListがnullptrです。");
		return;
	}

	if (!surfaceBuffer_) {
		LOG_ERROR("Material::SetGraphicsCommand メンバ変数surfaceBuffer_がnullptrです。Material::CreateBufferでsurfaceBufferを作成してください。");
		return;
	}

	commandList->SetGraphicsRootConstantBufferView(rootParamIndexMaterial, surfaceBuffer_->GetGPUVirtualAddress());

	if (textureData_) {
		commandList->SetGraphicsRootDescriptorTable(rootParamIndexTexture, textureData_->gpuHandle);
	}
}

void Material::SetTexture(const std::string& filePath) {
	textureData_ = &TextureManager::GetInstance()->Load(filePath);
}


