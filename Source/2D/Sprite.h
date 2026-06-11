#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include <memory>
#include <string>

#include "Material.h"
#include "Matrix4x4.h"
#include "Mesh.h"
#include "TransformationMatrix.h"
#include "Vector2.h"

/**
 * \class Sprite
 * \brief 2Dスプライトを管理すクラス
 */
class Sprite {
public:
	//--- デストラクタ ---

	~Sprite();

	//--- 公開関数 ---

	/**
	 * \brief 簡易的なスプライトを生成する
	 * \param[in] scale スプライトのスケール
	 * \param[in] textureFilePath テクスチャファイルパス
	 * \return 生成されたスプライト
	 */
	static std::unique_ptr<Sprite> CreateQuad(const Vector2& scale, const std::string& textureFilePath);

	/** \brief 更新処理 */
	void Update(const Matrix4x4& projectionMatrix);

	/** \brief 描画処理 */
	void Draw();

private:
	//--- コンストラクタ ---

	Sprite() = default;

	//--- 内部関数 ---

	/** \brief 初期化処理 */
	void Initialize();

	//--- メンバ変数 ---

	//2D用パラメータ
	Vector2 translation_ = { 0.0f, 0.0f };
	float rotation_ = 0.0f;
	Vector2 scale_ = { 1.0f, 1.0f };

	//座標変換リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//パーツへの参照・所有
	std::unique_ptr<Mesh> mesh_ = nullptr;
	std::unique_ptr<Material> material_ = nullptr;
};
