#include "Mesh.h"

#include <dxgiformat.h>

#include <string.h>
#include <numbers>
#include <cmath>

#include "D3D12Util.h"
#include "DirectXCommon.h"

std::unique_ptr<Mesh> Mesh::CreateSphere(uint32_t divisionVertical, uint32_t divisionHorizontal) {
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;

	const float kLonEvery = 2.0f * std::numbers::pi_v<float> / static_cast<float>(divisionHorizontal);
	const float kLatEvery = std::numbers::pi_v<float> / static_cast<float>(divisionVertical);

	//頂点データの作成
	for (uint32_t latIndex = 0; latIndex <= divisionVertical; ++latIndex) {
		//緯度の方向に分割
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex <= divisionHorizontal; ++lonIndex) {
			//軽度の方向に分割
			float lon = lonIndex * kLonEvery;

			VertexData vertex{};
			//座標計算
			vertex.position.x = std::cos(lat) * std::cos(lon);
			vertex.position.y = std::sin(lat);
			vertex.position.z = std::cos(lat) * std::sin(lon);
			vertex.position.w = 1.0f;

			//UV座標
			vertex.texCoord.x = static_cast<float>(lonIndex) / divisionHorizontal;
			vertex.texCoord.y = 1.0f - static_cast<float>(latIndex) / divisionVertical;

			//法線
			vertex.normal = { vertex.position.x, vertex.position.y, vertex.position.z };

			//配列の末尾にデータを入れる
			vertices.push_back(vertex);
		}
	}

	// インデックスデータの生成 (四角形を2つの三角形に分割)
	for (uint32_t latIndex = 0; latIndex < divisionVertical; ++latIndex) {
		for (uint32_t lonIndex = 0; lonIndex < divisionHorizontal; ++lonIndex) {
			//格子の左下の頂点番号を算出
			uint32_t start = latIndex * (divisionHorizontal + 1) + lonIndex;

			//1つ目の三角形(左下->左上->右上)
			indices.push_back(start);
			indices.push_back(start + (divisionHorizontal + 1));
			indices.push_back(start + 1);

			//2つ目の三角形(右上->左上->右下)
			indices.push_back(start + 1);
			indices.push_back(start + (divisionHorizontal + 1));
			indices.push_back(start + (divisionHorizontal + 1) + 1);
		}
	}

	auto mesh = std::make_unique<Mesh>();
	mesh->Initialize(vertices, indices);

	return mesh;
}

void Mesh::Bind(ID3D12GraphicsCommandList* commandList) const {
	//トポロジをセット(三角形をセット)
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点バッファのセット
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//インデックスバッファのセット(インデックスがある場合)
	if (indexCount_ > 0) {
		commandList->IASetIndexBuffer(&indexBufferView_);
	}
}

void Mesh::Initialize(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices) {
	vertexCount_ = static_cast<uint32_t>(vertices.size());
	indexCount_ = static_cast<uint32_t>(indices.size());

	auto device = DirectXCommon::GetInstance()->GetDevice();

	//頂点バッファの作成と転送
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * vertexCount_);
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexCount_;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	void* vertexPtr = nullptr;
	vertexResource_->Map(0, nullptr, &vertexPtr);
	std::memcpy(vertexPtr, vertices.data(), sizeof(VertexData) * vertexCount_);
	vertexResource_->Unmap(0, nullptr);

	//インデックスバッファの作成と転送(インデックスが存在する場合)
	if (indexCount_ > 0) {
		indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * indexCount_);
		indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
		indexBufferView_.SizeInBytes = sizeof(uint32_t) * indexCount_;
		indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

		void* indexPtr = nullptr;
		indexResource_->Map(0, nullptr, &indexPtr);
		std::memcpy(indexPtr, indices.data(), sizeof(uint32_t) * indexCount_);
		indexResource_->Unmap(0, nullptr);
	}
}
