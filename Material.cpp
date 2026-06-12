#include "Material.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"
#include "TextureManager.h"

#include <cassert>
#include <fstream>
#include <sstream>

Material::~Material() {
	if (constantBufferResource_ && constantBufferData_) {
		constantBufferResource_->Unmap(0, nullptr);
		constantBufferData_ = nullptr;
	}
}

std::unique_ptr<Material> Material::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//必要な変数を宣言
	std::unique_ptr<Material> material(new Material());
	std::string line; //ファイルから読み込んだ1行を格納
	std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
	assert(file.is_open()); //開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			material->property_.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	return material;
}

void Material::Bind(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial, UINT rootParamIndexTexture) {
	commandList->SetGraphicsRootConstantBufferView(rootParamIndexMaterial, constantBufferResource_->GetGPUVirtualAddress());

	if (textureData_) {
		commandList->SetGraphicsRootDescriptorTable(rootParamIndexTexture, textureData_->gpuHandle);
	}
}

void Material::SetTexture(const std::string& filePath) {
	textureData_ = &TextureManager::GetInstance()->Load(filePath);
}

void Material::Initialize() {
	constantBufferResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ConstantBufferData));
	constantBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&constantBufferData_));

	//デフォルト値を設定
	constantBufferData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	constantBufferData_->enableLighting = 1;
	constantBufferData_->uvTransform = Matrix4x4::Identity();

	//一度更新を行う
	Update();
}

void Material::Update() {
	if (!constantBufferData_) {
		return;
	}

	//uv座標変換データの計算
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrix = uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(uvTransform_.rotation.z);
	uvTransformMatrix = uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(uvTransform_.translation);
	constantBufferData_->uvTransform = uvTransformMatrix;
}
