#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "Render/Material.h"
#include "Render/Mesh.h"

#include <d3d12.h>
#include <memory>
#include <string>
#include <wrl/client.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// 3Dオブジェクトのインスタンスを管理するクラス
/// </summary>
class Model {
public:

	//GPUへ送るための座標変換行列データ
	struct TransformationMatrix {
		Matrix4x4 wvp;
		Matrix4x4 world;
	};

	//--- インスタンス管理 ---

	Model() = default;
	~Model();

	//--- 公開関数 ---

	/// <summary>
	/// 簡易的な球モデルを生成する
	/// </summary>
	/// <param name="textureFilePath"></param>
	/// <returns>生成された球モデル</returns>
	static std::unique_ptr<Model> CreateSphere(uint32_t divisionHorizontal = 16, uint32_t divisionVertical = 16);

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
	void Draw(const Transform& transform, const Matrix4x4& viewProjectionMatrix);

private:
	//--- 内部関数 ---

	/// <summary>
	/// バッファの生成
	/// </summary>
	/// <param name="device"></param>
	void CreateBuffer(ID3D12Device* device);

	void LoadOBJ(const std::string& filename);

	void LoadMaterialTemplateFile(const std::string& filename);

	//--- メンバ変数 ---

	//座標変換リソース
	ComPtr<ID3D12Resource> transformationBuffer_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//パーツへの参照・所有
	std::unique_ptr<Mesh> mesh_ = nullptr;
	std::unique_ptr<Material> material_ = nullptr;
};
