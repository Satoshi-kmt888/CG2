#pragma once

#include "Matrix4x4.h"
#include "Transform.h"
#include "Vector2.h"
#include "Vector4.h"

#include <d3d12.h>
#include <wrl/client.h>

/**
 * \struct VertexData
 * \brief 3Dオブジェクトの頂点情報を保持する構造体
 */
struct VertexData {
	Vector4 position; //<! 頂点座標
	Vector2	texCoord; //<! uv座標
};

/**
 * \class Object3d
 * \brief 3Dオブジェクトの描画とデータを管理するクラス
 * \details 頂点バッファ、マテリアル、WVPの行列リソース管理を行う
 * 描画に必要なSRVハンドルんp管理及びコマンドリストへの記録
 */
class Object3D {
public://--- ライフサイクル ---
	/**
	 * \brief 初期化処理
	 * \param[in] descriptorHeap SRVを管理するディスクリプターヒープ
	 */
	void Initialize(ID3D12DescriptorHeap* descriptorHeap);

	/**
	 * \brief 更新処理
	 * \param[in] viewProjectionMatrix ビュー・プロジェクション行列
	 */
	void Update(const Matrix4x4& viewProjectionMatrix);

	/** \brief 描画処理 */
	void Draw();

public://--- ゲッター ---
	D3D12_CPU_DESCRIPTOR_HANDLE GetTextureSrvHandleCPU() const { return textureSrvHandleCPU; }
	Vector4& GetMaterialData() const { return *materialData; }

private://--- メンバ変数 ---
	//GPUリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;   //!< 頂点バッファビリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource; //!< マテリアルバッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;      //!< WVP行列バッファリソース

	//ビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};       //<! 頂点バッファビュー
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU{}; //!< SRVのCPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU{}; //!< SRVのGPUハンドル

	//データポインタ
	VertexData* vertexData = nullptr; //!< CPU上の頂点データポインタ
	Vector4* materialData = nullptr;  //!< CPU上のマテリアルデータポインタ
	Matrix4x4* wvpData = nullptr;     //!< CPU上のWVP行列データポインタ

	//ワールド変換データ
	Transform transform{}; //!< オブジェクトのワールド座標データ
};
