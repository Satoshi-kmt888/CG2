#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <memory>
#include <string>

#include "Vector2.h"
#include "Vector4.h"
#include "TextureManager.h"

/**
 * \struct VertexData
 * \brief 3Dオブジェクトの頂点情報を保持する構造体
 */
struct VertexData {
	Vector4 position; //<! 頂点座標
	Vector2	texCoord; //<! uv座標
};

/**
 * \class Model
 * \brief 3Dモデルの形状データを管理するクラス
 * \details 頂点バッファの生成や保持を行い、複数のObject3Dから共有
 */
class Model{
public:
	//--- コンストラクタ・デストラクタ ---
	Model();
	~Model();

	//--- 公開関数 ---

	static std::unique_ptr<Model> CreateQuad();

	/**
	 * \brief 球体モデルのインスタンスを生成する
	 * \param divisionVertical 水平方向の分割数
	 * \param divisionHorizontal 垂直方向の分割数
	 * \return 生成されたModelのunique_ptr
	 */
	static std::unique_ptr<Model> CreateSphere(uint32_t divisionVertical = 16, uint32_t divisionHorizontal = 16);

	//--- ライフサイクル ---
	/**
	 * \brief 頂点バッファとテクスチャを描画コマンドにセットする
	 * \param commandList 転送コマンドの記録に使用するコマンドリスト
	 */
	void Draw(ID3D12GraphicsCommandList* commandList);

	//--- セッター ---
	void SetTexture(const std::string& filePath);

private:
	//--- 内部関数 ---
	/**
	 * \brief 頂点バッファリソースを作成し、Mapを行う
	 * \param vertexCount 生成する頂点数
	 */
	void CreateVertexBuffer(uint32_t vertexCount);

	//--- メンバ変数 ---

	//頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	uint32_t vertexCount_ = 0;
	VertexData* vertexData_ = nullptr;

	//テクスチャデータ
	const TextureData* textureData_ = nullptr;
};
