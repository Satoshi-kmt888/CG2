#include "Model.h"

#include "DirectXCommon.h"
#include "D3D12Util.h"

#include <numbers>
#include <cmath>

Model::Model(){

}

Model::~Model(){
	if (vertexResource_ && vertexData_) {
		vertexResource_->Unmap(0, nullptr);
	}
}

std::unique_ptr<Model> Model::CreateQuad(){
	std::unique_ptr<Model> model = std::make_unique<Model>();
	uint32_t vertexCount = 6;
	model->CreateVertexBuffer(vertexCount);
	model->vertexCount_ = vertexCount;

	//1枚目の三角形
	model->vertexData_[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };//左下
	model->vertexData_[0].texCoord = { 0.0f, 1.0f };
	model->vertexData_[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };//左上
	model->vertexData_[1].texCoord = { 0.0f, 0.0f };
	model->vertexData_[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };//右下
	model->vertexData_[2].texCoord = { 1.0f, 1.0f };

	//2枚目の三角形
	model->vertexData_[3].position = { 0.0f, 0.0f, 0.0f, 1.0f };//左上
	model->vertexData_[3].texCoord = { 0.0f, 0.0f };
	model->vertexData_[4].position = { 640.0f, 0.0f, 0.0f, 1.0f };//右上
	model->vertexData_[4].texCoord = { 1.0f, 0.0f };
	model->vertexData_[5].position = { 640.0f, 360.0f, 0.0f, 1.0f };//右下
	model->vertexData_[5].texCoord = { 1.0f, 1.0f };

	return model;
}

std::unique_ptr<Model> Model::CreateSphere(uint32_t divisionVertical, uint32_t divisionHorizontal) {
	std::unique_ptr<Model> model = std::make_unique<Model>();

	const float kLonEvery = 2.0f * std::numbers::pi_v<float> / static_cast<float>(divisionHorizontal);
	const float kLatEvery = std::numbers::pi_v<float> / static_cast<float>(divisionVertical);
	uint32_t vertexCount = divisionVertical * divisionHorizontal * 6;

	model->CreateVertexBuffer(vertexCount);
	model->vertexCount_ = vertexCount;

	//緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < divisionVertical; ++latIndex) {
		//現在の緯度
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;

		//経度の方向に分割
		for (uint32_t lonIndex = 0; lonIndex < divisionHorizontal; ++lonIndex) {
			uint32_t start = (latIndex * divisionHorizontal + lonIndex) * 6;
			//現在の経度
			float lon = lonIndex * kLonEvery;

			//
			float startU = static_cast<float>(lonIndex) / static_cast<float>(divisionHorizontal);
			float startV = 1.0f - static_cast<float>(latIndex) / static_cast<float>(divisionVertical);
			float nextU = static_cast<float>(lonIndex + 1) / static_cast<float>(divisionHorizontal);
			float nextV = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(divisionVertical);

			//左下
			model->vertexData_[start].position.x = std::cos(lat) * std::cos(lon);
			model->vertexData_[start].position.y = std::sin(lat);
			model->vertexData_[start].position.z = std::cos(lat) * std::sin(lon);
			model->vertexData_[start].position.w = 1.0f;
			model->vertexData_[start].texCoord = { startU, startV };

			//左上
			model->vertexData_[start + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			model->vertexData_[start + 1].position.y = std::sin(lat + kLatEvery);
			model->vertexData_[start + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			model->vertexData_[start + 1].position.w = 1.0f;
			model->vertexData_[start + 1].texCoord = { startU, nextV };

			//右下
			model->vertexData_[start + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			model->vertexData_[start + 2].position.y = std::sin(lat);
			model->vertexData_[start + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			model->vertexData_[start + 2].position.w = 1.0f;
			model->vertexData_[start + 2].texCoord = { nextU, startV };

			//右上
			model->vertexData_[start + 3].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			model->vertexData_[start + 3].position.y = std::sin(lat + kLatEvery);
			model->vertexData_[start + 3].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			model->vertexData_[start + 3].position.w = 1.0f;
			model->vertexData_[start + 3].texCoord = { nextU, nextV };

			//右下
			model->vertexData_[start + 4].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			model->vertexData_[start + 4].position.y = std::sin(lat);
			model->vertexData_[start + 4].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			model->vertexData_[start + 4].position.w = 1.0f;
			model->vertexData_[start + 4].texCoord = { nextU, startV };

			//左上
			model->vertexData_[start + 5].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			model->vertexData_[start + 5].position.y = std::sin(lat + kLatEvery);
			model->vertexData_[start + 5].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			model->vertexData_[start + 5].position.w = 1.0f;
			model->vertexData_[start + 5].texCoord = { startU, nextV };
		}
	}

	return model;
}

void Model::Draw(ID3D12GraphicsCommandList* commandList) {
	//頂点をセット
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	if (textureData_) {
		commandList->SetGraphicsRootDescriptorTable(2, textureData_->gpuHandle);
	}

	commandList->DrawInstanced(vertexCount_, 1, 0, 0);
}

void Model::SetTexture(const std::string& filePath){
	textureData_ = &TextureManager::GetInstance()->Load(filePath);
}

void Model::CreateVertexBuffer(uint32_t vertexCount) {
	//データ書き込み
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * vertexCount);
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexCount;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
}
