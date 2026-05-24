#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include <memory>
#include <string>

#include "Material.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "TransformationMatrix.h"

class Mesh;

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

	static std::unique_ptr<Model> CreateSphere(const std::string& textureFilePath);

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

private:
	//--- メンバ変数 ---

	//トランスフォーム
	Transform transform_{};

	//座標変換リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//パーツへの参照・所有
	std::unique_ptr<Mesh> mesh_ = nullptr;
	std::unique_ptr<Material> material_ = nullptr;
};
