#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include <memory>
#include <string>
#include <utility>

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

/**
 * \class Model
 * \brief
 */
class Model {
public:
	//初期化
	void Initialize();

	//更新
	void Update(const Matrix4x4& viewProjectionMatrix);

	//描画
	void Draw();

	void SetMesh(Mesh* mesh) { mesh_ = mesh; }
	void SetMaterial(std::unique_ptr<Material> material) { material_ = std::move(material); }
	void SetTexture(const std::string& filePath) { material_->SetTexture(filePath); }

private:
	//座標変換リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//パーツへの参照・所有
	Mesh* mesh_ = nullptr;
	std::unique_ptr<Material> material_ = nullptr;

	//トランスフォーム
	Transform transform_{};
};
