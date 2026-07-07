#pragma once

#include "Graphics/Material.h"
#include "Math/Matrix4x4.h"
#include "Graphics/Mesh.h"
#include "Math/Transform.h"

#include <d3d12.h>
#include <wrl/client.h>
#include <memory>
#include <string>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// 3Dオブジェクトのインスタンスを管理するクラス
/// </summary>
class Model {
public:

	/// <summary>
	/// GPUへ送るための座標変換行列データ
	/// </summary>
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
	/// <returns></returns>
	static std::unique_ptr<Model> CreateSphere(
		uint32_t divisionHorizontal = 16, uint32_t divisionVertical = 16,
		const std::string& textureFilePath = "resources/uvChecker.png");

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="viewProjectionMatrix"></param>
	void Update(const Matrix4x4& viewProjectionMatrix);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	//--- 内部関数 ---

	/// <summary>
	/// 
	/// </summary>
	void Build();

	//--- メンバ変数 ---

	//トランスフォーム
	Transform transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	//座標変換リソース
	ComPtr<ID3D12Resource> transformationBuffer_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//パーツへの参照・所有
	std::unique_ptr<Mesh> mesh_ = nullptr;
	std::unique_ptr<Material> material_ = nullptr;
};
