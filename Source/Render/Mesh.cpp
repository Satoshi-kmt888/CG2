#include "Render/Mesh.h"

#include "Graphics/D3D12Utility.h"
#include "Logging/Logger.h"

#include <dxgiformat.h>
#include <cstdint>

void Mesh::AddVertex(const VertexData& vertex) {
	vertices_.emplace_back(vertex);
	vertexCount_ = static_cast<UINT>(vertices_.size());
}

void Mesh::AddIndex(uint32_t index) {
	indices_.emplace_back(index);
	indexCount_ = static_cast<UINT>(indices_.size());
}

void Mesh::CreateBuffer(ID3D12Device* device) {
	if (!device) {
		LOG_ERROR("Mesh::CreateBuffer 引数deviceがnullptrです。");
		return;
	}

	if (vertices_.empty()) {
		LOG_ERROR("Mesh::CreateBuffer メンバ変数vertices_に頂点データがありません。AddVertexで頂点データを追加してください。");
		return;
	}

	//頂点バッファの作成
	size_t vertexSize = sizeof(VertexData) * vertices_.size();
	vertexBuffer_ = D3D12Utility::CreateBufferResource(device, vertexSize);

	//頂点バッファの転送
	void* vertexPtr = nullptr;
	vertexBuffer_->Map(0, nullptr, &vertexPtr);
	std::memcpy(vertexPtr, vertices_.data(), vertexSize);
	vertexBuffer_->Unmap(0, nullptr);

	//ビューの設定
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast<UINT>(vertexSize);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//インデックスバッファの作成と転送(インデックスが存在する場合)
	if (indexCount_ > 0) {
		//インデックスバッファの作成
		size_t indexSize = sizeof(uint32_t) * indices_.size();
		indexBuffer_ = D3D12Utility::CreateBufferResource(device, indexSize);

		//インデックスバッファの転送
		void* indexPtr = nullptr;
		indexBuffer_->Map(0, nullptr, &indexPtr);
		std::memcpy(indexPtr, indices_.data(), indexSize);
		indexBuffer_->Unmap(0, nullptr);

		//ビューの設定
		indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
		indexBufferView_.SizeInBytes = static_cast<UINT>(indexSize);
		indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	}

	//バッファを作成し終えたので配列の中身を空にする
	vertices_.clear();
	indices_.clear();
}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList) const {
	if (!commandList) {
		LOG_ERROR("Mesh::Draw 引数commandListがnullptrです。");
		return;
	}

	if (!vertexBuffer_) {
		LOG_ERROR("Mesh::Draw メンバ変数vertexBuffer_がnullptrです。Mesh::CreateBufferでvertexBuffer_を作成してください。");
		return;
	}

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //頂点の結び方
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_); //頂点バッファのセット

	//インデックスの有無でドローコールを行う
	if (indexCount_ > 0) {
		commandList->IASetIndexBuffer(&indexBufferView_);
		commandList->DrawIndexedInstanced(indexCount_, 1, 0, 0, 0);
	} else {
		commandList->DrawInstanced(vertexCount_, 1, 0, 0);
	}
}
