#include "Material.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"
#include "TextureManager.h"

Material::~Material() {
	if (resource_ && materialData_) {
		resource_->Unmap(0, nullptr);
	}
}

void Material::Initialize() {
	resource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(MaterialData));
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	//デフォルト値を設定
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = 1;
	materialData_->uvTransform = Matrix4x4::Identity();

	Update();
}

void Material::Update(){
	if (!materialData_) {
		return;
	}

	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrix = uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(uvTransform_.rotation.z);
	uvTransformMatrix = uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(uvTransform_.translation);
	materialData_->uvTransform = uvTransformMatrix;
}

void Material::SetTexture(const std::string& filePath) {
	textureData_ = &TextureManager::GetInstance()->Load(filePath);
}

void Material::Bind(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial, UINT rootParamIndexTexture) {
	commandList->SetGraphicsRootConstantBufferView(rootParamIndexMaterial, resource_->GetGPUVirtualAddress());

	if (textureData_) {
		commandList->SetGraphicsRootDescriptorTable(rootParamIndexTexture, textureData_->gpuHandle);
	}
}
