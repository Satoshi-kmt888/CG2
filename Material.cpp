#include "Material.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"
#include "TextureManager.h"

Material::~Material() {
	if (constantBufferResource_ && constantBufferData_) {
		constantBufferResource_->Unmap(0, nullptr);
	}
}

void Material::Initialize() {
	constantBufferResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ConstantBufferData));
	constantBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&constantBufferData_));

	//デフォルト値を設定
	constantBufferData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	constantBufferData_->enableLighting = 1;
	constantBufferData_->uvTransform = Matrix4x4::Identity();

	Update();
}

void Material::Update(){
	if (!constantBufferData_) {
		return;
	}

	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrix = uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(uvTransform_.rotation.z);
	uvTransformMatrix = uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(uvTransform_.translation);
	constantBufferData_->uvTransform = uvTransformMatrix;
}

void Material::SetTexture(const std::string& filePath) {
	textureData_ = &TextureManager::GetInstance()->Load(filePath);
}

void Material::Bind(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial, UINT rootParamIndexTexture) {
	commandList->SetGraphicsRootConstantBufferView(rootParamIndexMaterial, constantBufferResource_->GetGPUVirtualAddress());

	if (textureData_) {
		commandList->SetGraphicsRootDescriptorTable(rootParamIndexTexture, textureData_->gpuHandle);
	}
}
