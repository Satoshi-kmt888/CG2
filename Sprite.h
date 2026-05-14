#pragma once
#include <memory>

#include "Material.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "TransformationMatrix.h"

class Mesh;

/**
 * \class Sprite
 */
class Sprite {
public:
	//--- コンストラクタ・デストラクタ ---

	Sprite() = default;
	~Sprite() = default;

	//--- 公開関数 ---

	/**
	 * \brief 初期化
	 */
	void Initialize();

	/**
	 * \brief 更新処理
	 * 
	 */
	void Update(const Matrix4x4& projectionMatrix);

	/**
	 * \brief 描画処理
	 */
	void Draw();

	//--- セッター ---

	void SetMesh(Mesh* mesh) { mesh_ = mesh; }
	void SetMaterial(std::unique_ptr<Material> material) { material_ = std::move(material); }
	void SetTexture(const std::string& filePath) { material_->SetTexture(filePath); }

private:
	//--- メンバ変数 ---

	//2D用パラメータ
	Vector2 position_ = { 0.0f, 0.0f };
	float rotation_ = 0.0f;
	Vector2 size_ = { 512.0f, 512.0f };

	//座標変換リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//パーツへの参照・所有
	Mesh* mesh_ = nullptr;
	std::unique_ptr<Material> material_ = nullptr;
};
