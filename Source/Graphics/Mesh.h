#pragma once

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <vector>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
///  GPU上の頂点バッファ・インデックスバッファを管理するクラス
/// </summary>
class Mesh {
public:
	//--- 内部データ構造体 ---

	//頂点1つ分が保持するデータ構造体
	struct VertexData {
		Vector4 position; //ローカル座標
		Vector2 texCoord; //uv座標
		Vector3 normal;   //法線ベクトル
	};

	//--- インスタンス管理 ---

	Mesh() = default;
	~Mesh() = default;

	//--- 公開関数 ---

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
	/// 頂点データをもとにバッファを生成
	/// </summary>
	/// <param name="device"></param>
	void Build(ID3D12Device* device);

	/// <summary>
	/// コマンドリストに紐づけ
	/// </summary>
	/// <param name="commandList"></param>
	void Bind(ID3D12GraphicsCommandList* commandList) const;

	/// <summary>
	/// ドローコールを実行
	/// </summary>
	/// <param name="commandList"></param>
	void Draw(ID3D12GraphicsCommandList* commandList) const;

private:
	//--- メンバ変数 ---

	//頂点バッファ
	ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	std::vector<VertexData> vertices_{};

	//インデックスバッファ
	ComPtr<ID3D12Resource> indexResource_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	std::vector<uint32_t> indices_{};
};
