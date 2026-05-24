#pragma once
#include <d3d12.h>
#include <Windows.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>
#include <string>

#include "Matrix4x4.h"
#include "Transform.h"
#include "Vector3.h"
#include "Vector4.h"

struct TextureData;

/**
 * \class Material
 * \brief 物体の質感、色、テクスチャを管理するクラス
 */
class Material {
public:
	//--- 内部データ構造体 ---

	/**
    * \struct ConstantBufferData
    * \brief GPUへ送るためのマテリアルデータ構造体
    */
	struct ConstantBufferData {
		Vector4 color;          //<! RGBAの色データ
		int32_t enableLighting; //<! ライティング有効フラグ
		float padding[3];       //<! パディング
		Matrix4x4 uvTransform;  //<! uv座標変換データ
	};

	/**
	 * \struct Property
	 * \brief マテリアルが保持するリソースデータ構造体
	 */
	struct Property {
		std::string textureFilePath;
	};

	//--- コンストラクタ・デストラクタ ---

	Material() = default;
	~Material();

	//--- 公開関数 ---

	/**
	 * \brief マテリアルの初期化
	 * \details 定数バッファの生成とMap、デフォルト値の設定
	 */
	void Initialize();

	void Update();

	/**
	 * \brief 描画コマンドのバインド
	 * \param[in] commandList コマンドリスト
	 * \param[in] rootParamIndexMaterial マテリアル用ルートパラメータ番号
	 * \param[in] rootParamIndexTexture テクスチャ用ルートパラメータ番号
	 */
	void Bind(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial, UINT rootParamIndexTexture);

	/**
	 * \brief OBJファイルを読み込み、テクスチャデータを抽出
	 * \param[in] directoryPath ディレクトリファイルのパス
	 * \param[in] filename ファイル名(.objも含む)
	 * \return テクスチャデータを格納したMaterialポインタ
	 */
	static std::unique_ptr<Material> LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	//--- ゲッター ---

	const Property& GetProperty() const { return property_; }

	const Transform& GetUVTransform() const { return uvTransform_; }
	Vector3 GetUVScale() const { return uvTransform_.scale; }
	Vector3 GetUVRotation() const { return uvTransform_.rotation; }
	Vector3 GetUVTranslation() const { return uvTransform_.translation; }

	//--- セッター ---

	void SetUVTransform(const Transform& uvTransform) { uvTransform_ = uvTransform; }

	void SetColor(const Vector4& color) { constantBufferData_->color = color; }
	void SetEnableLighting(const uint32_t enableLighting) { constantBufferData_->enableLighting = enableLighting; }
	void SetTexture(const std::string& filePath);

private:
	//--- メンバ変数 ---

	Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferResource_ = nullptr;
	ConstantBufferData* constantBufferData_ = nullptr;
	Property property_;
	const TextureData* textureData_ = nullptr;

	//uv座標変換データ
	Transform uvTransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
};
