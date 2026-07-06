#include "Material.h"

#include "D3D12Utility.h"
#include "TextureManager.h"

Material::~Material() {
	if (surfaceBuffer_ && surfaceData_) {
		surfaceBuffer_->Unmap(0, nullptr);
		surfaceData_ = nullptr;
	}
}

void Material::Build(ID3D12Device* device) {
	surfaceBuffer_ = D3D12Utility::CreateBufferResource(device, sizeof(SurfaceData));
	void* materialPtr = nullptr;
	surfaceBuffer_->Map(0, nullptr, &materialPtr);

	surfaceData_ = static_cast<SurfaceData*>(materialPtr);
	if (surfaceData_) {
		*surfaceData_ = SurfaceData();
	}
}

void Material::Bind(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial, UINT rootParamIndexTexture) {
	commandList->SetGraphicsRootConstantBufferView(rootParamIndexMaterial, surfaceBuffer_->GetGPUVirtualAddress());

	if (textureData_) {
		commandList->SetGraphicsRootDescriptorTable(rootParamIndexTexture, textureData_->gpuHandle);
	}
}

void Material::SetTexture(const std::string& filePath) {
	TextureManager::GetInstance()->Load(filePath);
}

void Material::Update() {
	if (!surfaceData_) {
		return;
	}
}
