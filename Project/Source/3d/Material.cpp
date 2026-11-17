#include "Material.h"

#include "Debugger/Logger.h"
#include "Graphics/D3D12Utility.h"
#include "Graphics/TextureManager.h"
#include "Math/Matrix4x4.h"

bool Material::Initialize(ID3D12Device* device) {
	if (!m_buffer.Create(device)) {
		LOG_ERROR("定数バッファの生成に失敗しました。");
		return false;
	}

	return true;
}

void Material::Update() {
	//uv座標変換データの計算
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeAffine(transform_.scale, transform_.rotation, transform_.translation);
	m_buffer->uvTransform = uvTransformMatrix;
}

void Material::SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial, UINT rootParamIndexTexture) const {
	if (!commandList) {
		LOG_ERROR("Material::SetGraphicsCommand 引数commandListがnullptrです。");
		return;
	}

	commandList->SetGraphicsRootConstantBufferView(rootParamIndexMaterial, m_buffer.GetGPUAddress());

	if (textureData_) {
		commandList->SetGraphicsRootDescriptorTable(rootParamIndexTexture,
			textureData_->descriptor_handle.gpu_handle);
	}
}

void Material::SetTexture(const std::string& filePath) {
	textureData_ = &TextureManager::GetInstance()->Load(filePath);
}
