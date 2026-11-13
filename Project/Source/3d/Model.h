#pragma once

#include "3d/Material.h"
#include "3d/Mesh.h"
#include "Math/Matrix4x4.h"
#include "Math/WorldTransform.h"

#include <d3d12.h>
#include <memory>
#include <string>
#include <wrl/client.h>

/// <summary>
/// 3Dオブジェクトのインスタンスを管理するクラス
/// </summary>
class Model {
public:
	struct TransformationMatrix {
		Matrix4x4 wvp;
		Matrix4x4 world;
	};

	//=========================================================
	// public methods
	//=========================================================

	Model() = default;
	~Model();

	static std::unique_ptr<Model> CreatePlane(const std::string& filename);

	/// <summary>
	/// 簡易的な球モデルを生成する
	/// </summary>
	/// <param name="textureFilePath"></param>
	/// <returns>生成された球モデル</returns>
	static std::unique_ptr<Model> CreateSphere(const std::string& filename, uint32_t division_horizontal = 16, uint32_t division_vertical = 16);

	/// <summary>
	/// OBJファイルからモデルを生成する
	/// </summary>
	/// <param name="filename"></param>
	/// <returns>生成されたモデル</returns>
	static std::unique_ptr<Model> CreateFromOBJ(const std::string& filename);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="viewProjectionMatrix"></param>
	void Draw(const WorldTransform& transform, const Matrix4x4& view_projection_matrix);

	//=========================================================
	// accessor
	//=========================================================

	const Vector4& GetColor() const { return m_material->GetColor(); }
	uint32_t GetLightType() const { return m_material->GetLightType(); }
	const WorldTransform& GetUVTransform() const { return m_material->GetTransform(); }

	//---------------------------------------------------------

	void SetColor(const Vector4& color) { m_material->SetColor(color); }
	void SetLightType(uint32_t lightType) { m_material->SetLightType(lightType); }
	void SetUVTransform(const WorldTransform& transform) { m_material->SetTransform(transform); }

private:
	//=========================================================
	// private methods
	//=========================================================

	/// <summary>
	/// バッファの生成
	/// </summary>
	/// <param name="device"></param>
	void CreateBuffer(ID3D12Device* device);

	void LoadOBJ(const std::string& filename);

	void LoadMaterialTemplateFile(const std::string& filename);

	//=========================================================
	// private variables
	//=========================================================

	//座標変換リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> m_transformation_buffer = nullptr;
	TransformationMatrix* m_transformation_data = nullptr;

	//パーツへの参照・所有
	std::unique_ptr<Mesh> m_mesh = nullptr;
	std::unique_ptr<Material> m_material = nullptr;
};
