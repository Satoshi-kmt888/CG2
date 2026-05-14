#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include <memory>
#include <string>

#include "Material.h"
#include "Matrix4x4.h"
#include "Transform.h"

/**
 * \struct TransformationMatrix
 * \brief GPUへ送るための座標変換行列データ構造体
 */
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

class Mesh;
class Material;

/**
 * \class Model
 * \brief 3Dオブジェクトのインスタンスを管理するクラス
 */
class Model {
public:
	//--- コンストラクタ・デストラクタ ---

	Model() = default;
	~Model() = default;

	//--- 公開関数 ---

	/**
	 * \brief 初期化処理
	 */
	void Initialize();

	/**
	 * \brief 更新処理
	 * \param[in] viewProjectionMatrix ビュー・プロジェクション行列
	 */
	void Update(const Matrix4x4& viewProjectionMatrix);

	/**
	 * \brief 描画処理
	 * \details セットされたMeshとMaterialを使用して描画コマンドを積む
	 */
	void Draw();

	//--- セッター ---

	void SetMesh(Mesh* mesh) { mesh_ = mesh; }
	void SetMaterial(std::unique_ptr<Material> material) { material_ = std::move(material); }
	void SetTexture(const std::string& filePath) { material_->SetTexture(filePath); }

private:
	//--- メンバ変数 ---

	//トランスフォーム
	Transform transform_{};

	//座標変換リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//パーツへの参照・所有
	Mesh* mesh_ = nullptr;
	std::unique_ptr<Material> material_ = nullptr;
};
