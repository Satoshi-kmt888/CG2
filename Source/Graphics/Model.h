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
#include "Vector3.h"

/**
 * \class Model
 * \brief 3Dオブジェクトのインスタンスを管理するクラス
 */
class Model {
public:
	//--- デストラクタ ---

	~Model();

	//--- 公開関数 ---

	/**
	 * \brief 簡易的な球モデルを生成する
	 * \param[in] textureFilePath テクスチャファイルパス
	 * \return 生成された球モデル
	 */
	static std::unique_ptr<Model> CreateSphere(const std::string& textureFilePath);

	/**
	 * \brief OBJファイルからモデルを生成する
	 * \param[in] filename ファイル名(.objを含む)
	 * \return 生成されたモデル
	 */
	static std::unique_ptr<Model> CreateFromObj(const std::string& filename);

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

	const Vector3& GetScale() const { return transform_.scale; }
	const Vector3& GetRotation() const { return transform_.rotation; }
	const Vector3& GetTranslation() const { return transform_.translation; }

	//--- セッター ---

	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetRotation(const Vector3& rotation) { transform_.rotation = rotation; }
	void SetTranslation(const Vector3& translation) { transform_.translation = translation; }

private:
	//--- コンストラクタ ---

	Model() = default;

	//--- 内部関数 ---

	/** \brief 初期化処理 */
	void Initialize();

	//--- メンバ変数 ---

	//トランスフォーム
	Transform transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f} };

	//座標変換リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//パーツへの参照・所有
	std::unique_ptr<Mesh> mesh_ = nullptr;
	std::unique_ptr<Material> material_ = nullptr;
};
