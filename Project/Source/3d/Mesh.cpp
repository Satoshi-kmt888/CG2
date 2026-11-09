#include "Mesh.h"

#include "Debugger/Logger.h"
#include "Graphics/D3D12Utility.h"

#include <cstdint>
#include <dxgiformat.h>

void Mesh::AddVertex(const VertexData& vertex) {
	m_vertices.emplace_back(vertex);
	m_vertex_count = static_cast<UINT>(m_vertices.size());
}

void Mesh::AddIndex(uint32_t index) {
	m_indices.emplace_back(index);
	m_index_count = static_cast<UINT>(m_indices.size());
}

void Mesh::CreateBuffer(ID3D12Device* device) {
	if (!device) {
		LOG_ERROR("Mesh::CreateBuffer 引数deviceがnullptrです。");
		return;
	}

	if (m_vertices.empty()) {
		LOG_ERROR("Mesh::CreateBuffer メンバ変数vertices_に頂点データがありません。AddVertexで頂点データを追加してください。");
		return;
	}

	//頂点バッファの作成
	size_t vertex_size = sizeof(VertexData) * m_vertices.size();
	m_vertex_buffer = D3D12Utility::CreateBufferResource(device, vertex_size);

	//頂点バッファの転送
	void* vertex_ptr = nullptr;
	m_vertex_buffer->Map(0, nullptr, &vertex_ptr);
	std::memcpy(vertex_ptr, m_vertices.data(), vertex_size);
	m_vertex_buffer->Unmap(0, nullptr);

	//ビューの設定
	m_vertex_buffer_view.BufferLocation = m_vertex_buffer->GetGPUVirtualAddress();
	m_vertex_buffer_view.SizeInBytes = static_cast<UINT>(vertex_size);
	m_vertex_buffer_view.StrideInBytes = sizeof(VertexData);

	//インデックスバッファの作成と転送(インデックスが存在する場合)
	if (m_index_count > 0) {
		//インデックスバッファの作成
		size_t index_size = sizeof(uint32_t) * m_indices.size();
		m_index_buffer = D3D12Utility::CreateBufferResource(device, index_size);

		//インデックスバッファの転送
		void* index_ptr = nullptr;
		m_index_buffer->Map(0, nullptr, &index_ptr);
		std::memcpy(index_ptr, m_indices.data(), index_size);
		m_index_buffer->Unmap(0, nullptr);

		//ビューの設定
		m_index_buffer_view.BufferLocation = m_index_buffer->GetGPUVirtualAddress();
		m_index_buffer_view.SizeInBytes = static_cast<UINT>(index_size);
		m_index_buffer_view.Format = DXGI_FORMAT_R32_UINT;
	}

	//バッファを作成し終えたので配列の中身を空にする
	m_vertices.clear();
	m_indices.clear();
}

void Mesh::Draw(ID3D12GraphicsCommandList* command_list) const {
	if (!command_list) {
		LOG_ERROR("Mesh::Draw 引数commandListがnullptrです。");
		return;
	}

	if (!m_vertex_buffer) {
		LOG_ERROR("Mesh::Draw メンバ変数vertexBuffer_がnullptrです。Mesh::CreateBufferでvertexBuffer_を作成してください。");
		return;
	}

	command_list->IASetVertexBuffers(0, 1, &m_vertex_buffer_view); //頂点バッファのセット

	//インデックスの有無でドローコールを行う
	if (m_index_count > 0) {
		command_list->IASetIndexBuffer(&m_index_buffer_view);
		command_list->DrawIndexedInstanced(m_index_count, 1, 0, 0, 0);
	} else {
		command_list->DrawInstanced(m_vertex_count, 1, 0, 0);
	}
}

void Mesh::Draw(ID3D12GraphicsCommandList* command_list, UINT instance_count) {
	if (!command_list) {
		LOG_ERROR("Mesh::Draw 引数commandListがnullptrです。");
		return;
	}

	if (!m_vertex_buffer) {
		LOG_ERROR("Mesh::Draw メンバ変数vertexBuffer_がnullptrです。Mesh::CreateBufferでvertexBuffer_を作成してください。");
		return;
	}

	command_list->IASetVertexBuffers(0, 1, &m_vertex_buffer_view); //頂点バッファのセット

	//インデックスの有無でドローコールを行う
	if (m_index_count > 0) {
		command_list->IASetIndexBuffer(&m_index_buffer_view);
		command_list->DrawIndexedInstanced(m_index_count, instance_count, 0, 0, 0);
	} else {
		command_list->DrawInstanced(m_vertex_count, instance_count, 0, 0);
	}
}
