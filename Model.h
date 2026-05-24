#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include <memory>
#include <string>

#include "Material.h"
#include "Matrix4x4.h"
#include "Mesh.h"
#include "Transform.h"
#include "TransformationMatrix.h"

/**
 * \class Model
 * \brief 3Dオブジェクトのインスタンスを管理するクラス
 */
class Model {
public:

	struct MaterialData {
		std::string textureFilePath;
	};

	//--- コンストラクタ・デストラクタ ---

	Model() = default;
	~Model() = default;

	//--- 公開関数 ---

	/**
	 * \brief 簡易的な球モデルを生成する
	 * \param textureFilePath テクスチャファイルパス
	 * \return 生成された球モデルデータ
	 */
	static std::unique_ptr<Model> CreateSphere(const std::string& textureFilePath);

	/**
	 * \brief OBJファイルからモデルを生成する
	 * \param filename ファイル名(.objを含む)
	 * \return 生成されたモデル
	 */
	static std::unique_ptr<Model> CreateFromObj(const std::string& filename);

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

	//--- ゲッター ---

	const Vector3& GetRotation() const { return transform_.rotation; }

	//--- セッター ---

	void SetRotation(const Vector3& rotation) { transform_.rotation = rotation; }

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
