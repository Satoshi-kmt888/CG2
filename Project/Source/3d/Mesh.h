#pragma once

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <cstdint>
#include <d3d12.h>
#include <vector>
#include <wrl/client.h>

/// <summary>
///  GPU上の頂点バッファ・インデックスバッファを管理するクラス
/// </summary>
class Mesh {
public:
	//頂点1つ分が保持するデータ構造体
	struct VertexData {
		Vector4 position; //ローカル座標
		Vector2 texcoord; //uv座標
		Vector3 normal;   //法線ベクトル
	};

	//=========================================================
	// public methods
	//=========================================================

	Mesh() = default;
	~Mesh() = default;

	/// <summary>
	/// 頂点データの追加
	/// </summary>
	/// <param name="vertex"></param>
	void AddVertex(const VertexData& vertex);

	/// <summary>
	/// インデックスの追加
	/// </summary>
	/// <param name="index"></param>
	void AddIndex(uint32_t index);

	/// <summary>
	/// バッファの生成
	/// </summary>
	/// <param name="device"></param>
	void CreateBuffer(ID3D12Device* device);

	/// <summary>
	/// ドローコールを実行
	/// </summary>
	/// <param name="commandList"></param>
	void Draw(ID3D12GraphicsCommandList* command_list) const;

	/// <summary>
	/// インスタンシング描画用
	/// </summary>
	/// <param name="command_list"></param>
	/// <param name="instance_count"></param>
	void Draw(ID3D12GraphicsCommandList* command_list, UINT instance_count);

private:
	//=========================================================
	// private variables
	//=========================================================

	//頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertex_buffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vertex_buffer_view{};
	std::vector<VertexData> m_vertices{};
	UINT m_vertex_count = 0;

	//インデックス
	Microsoft::WRL::ComPtr<ID3D12Resource> m_index_buffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW m_index_buffer_view{};
	std::vector<uint32_t> m_indices{};
	UINT m_index_count = 0;
};
