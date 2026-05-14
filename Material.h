#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <string>

#include "Vector4.h"

/**
 * \struct MaterialData
 * \brief GPUへ送るためのマテリアルデータ構造体
 */
struct MaterialData {
	Vector4 color;          //<! RGBAの色データ
	int32_t enableLighting; //<! ライティング有効フラグ
};

struct TextureData;

/**
 * \class Material
 * \brief
 */
class Material {
public:
	//コンストラクタ・デストラクタ
	Material() = default;
	~Material();

	//初期化
	void Initialize();

	//描画準備
	void Bind(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial, UINT rootParamIndexTexture);

	void SetColor(const Vector4& color) { materialData_->color = color; }
	void SetTexture(const std::string& filePath);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
	MaterialData* materialData_ = nullptr;
	const TextureData* textureData_ = nullptr;
};

