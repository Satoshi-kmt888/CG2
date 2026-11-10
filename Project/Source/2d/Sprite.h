#pragma once

#include "3d/Material.h"
#include "3d/Mesh.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector2.h"
#include "Math/WorldTransform.h"

#include <d3d12.h>
#include <memory>
#include <string>
#include <wrl/client.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// 2Dスプライトを管理すクラス
/// </summary>
class Sprite {
public:
	//GPUへ送るための座標変換行列データ
	struct TransformationMatrix {
		Matrix4x4 wvp;
		Matrix4x4 world;
	};

	//=========================================================
	// public methods
	//=========================================================

	Sprite() = default;
	~Sprite();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="viewProjectionMatrix"></param>
	void Draw(WorldTransform& transform, const Matrix4x4& viewProjectionMatrix);

	/// <summary>
	/// 通常のスプライトを生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Sprite> Create();

	//=========================================================
	// accessor
	//=========================================================

	const Vector4& GetColor() const { return material_->GetColor(); }
	uint32_t GetLightType() const { return material_->GetLightType(); }
	const WorldTransform& GetUVTransform() const { return material_->GetTransform(); }

	//---------------------------------------------------------

	void SetColor(const Vector4& color) { material_->SetColor(color); }
	void SetLightType(uint32_t lightType) { material_->SetLightType(lightType); }
	void SetUVTransform(const WorldTransform& transform) { material_->SetTransform(transform); }

private:
	//=========================================================
	// private methods
	//=========================================================

	void CreateBuffer(ID3D12Device* device);

	//=========================================================
	// private variables
	//=========================================================

	//座標変換リソース
	ComPtr<ID3D12Resource> transformationBuffer_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//パーツへの参照
	std::unique_ptr<Mesh> mesh_ = nullptr;
	std::unique_ptr<Material> material_ = nullptr;
};
