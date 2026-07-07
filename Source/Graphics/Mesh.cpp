#include "Mesh.h"

#include "Graphics/D3D12Utility.h"
#include "Logging/Logger.h"

#include <dxgiformat.h>
#include <cstdint>

void Mesh::AddVertex(const VertexData& vertex) {
	vertices_.emplace_back(vertex);
}

void Mesh::AddIndex(uint32_t index) {
	indices_.emplace_back(index);
}

void Mesh::Build(ID3D12Device* device) {
	if (device == nullptr) {
		LOG_ERROR("device is null.");
		return;
	}

	if (vertices_.empty()) {
		LOG_ERROR("vertices is empty. Cannot build mesh.");
		return;
	}

	//頂点バッファの作成
	size_t vertexSize = sizeof(VertexData) * vertices_.size();
	vertexResource_ = D3D12Utility::CreateBufferResource(device, vertexSize);

	//頂点バッファの転送
	void* vertexPtr = nullptr;
	vertexResource_->Map(0, nullptr, &vertexPtr);
	std::memcpy(vertexPtr, vertices_.data(), vertexSize);
	vertexResource_->Unmap(0, nullptr);

	//ビューの設定
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast<UINT>(vertexSize);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//インデックスバッファの作成と転送(インデックスが存在する場合)
	if (!indices_.empty()) {
		//インデックスバッファの作成
		size_t indexSize = sizeof(uint32_t) * indices_.size();
		indexResource_ = D3D12Utility::CreateBufferResource(device, indexSize);

		//インデックスバッファの転送
		void* indexPtr = nullptr;
		indexResource_->Map(0, nullptr, &indexPtr);
		std::memcpy(indexPtr, indices_.data(), indexSize);
		indexResource_->Unmap(0, nullptr);

		//ビューの設定
		indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
		indexBufferView_.SizeInBytes = static_cast<UINT>(indexSize);
		indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	}
}

void Mesh::Bind(ID3D12GraphicsCommandList* commandList) const {
	if (commandList == nullptr) {
		return;
	}

	//トポロジをセット(三角形をセット)
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点バッファのセット
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//インデックスバッファのセット(インデックスがある場合)
	if (!indices_.empty()) {
		commandList->IASetIndexBuffer(&indexBufferView_);
	}
}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList) const {
	if (commandList == nullptr) {
		LOG_ERROR("commandList is null.");
		return;
	}

	if (vertexResource_ == nullptr) {
		LOG_ERROR("vertexResource is null.");
		return;
	}

	if (indices_.empty()) {
		commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
	} else {
		commandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
	}
}
