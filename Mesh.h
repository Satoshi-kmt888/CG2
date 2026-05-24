#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

/**
 * \class Mesh
 * \brief GPU上の頂点バッファ・インデックスバッファを管理するクラス
 */
class Mesh {
public:
	//--- 内部データ構造体 ---

	/**
	* \struct VertexData
	* \brief 頂点1つ分が保持するデータ構造体
	*/
	struct VertexData {
		Vector4 position; //<! ローカル座標
		Vector2 texCoord; //<! uv座標
		Vector3 normal;   //<! 法線ベクトル
	};

	//--- コンストラクタ・デストラクタ ---

	Mesh() = default;
	~Mesh() = default;

	//--- 公開関数 ---

	/**
	 * \brief 矩形メッシュを生成
	 * \return
	 */
	static std::unique_ptr<Mesh> CreateQuad();

	/**
	 * \brief 球メッシュの生成
	 * \param[in] divisionVertical 垂直方向の分割数
	 * \param[in] divisionHorizontal 水平方向の分割数
	 * \return
	 */
	static std::unique_ptr<Mesh> CreateSphere(uint32_t divisionVertical = 16, uint32_t divisionHorizontal = 16);

	/**
	 * \brief コマンドリストにバッファをセットし、描画準備を行う
	 * \param[in] commandList セット対象のコマンドリスト
	 */
	void Bind(ID3D12GraphicsCommandList* commandList) const;

	/**
	 * \brief 頂点とインデックスのバッファ生成
	 */
	void CreateBuffers();

	//--- ゲッター ---

	const std::vector<VertexData>& GetVertices() const { return vertices_; }
	const std::vector<uint32_t>& GetIndices() const { return indices_; }

private:
	//--- メンバ変数 ---

	//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	std::vector<VertexData> vertices_{};

	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	std::vector<uint32_t> indices_{};
};
