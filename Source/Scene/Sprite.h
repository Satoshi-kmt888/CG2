#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector2.h"
#include "Math/Transform.h"
#include "Scene/Material.h"
#include "Scene/Mesh.h"

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
	//--- 内部データ ---

	//GPUへ送るための座標変換行列データ
	struct TransformationMatrix {
		Matrix4x4 wvp;
		Matrix4x4 world;
	};

	//--- インスタンス管理 ---

	Sprite() = default;
	~Sprite();

	//--- 公開関数

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="viewProjectionMatrix"></param>
	void Draw(Transform& transform, const Matrix4x4& viewProjectionMatrix);

	/// <summary>
	/// 通常のスプライトを生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Sprite> Create();

private:
	//--- 内部関数 ---

	void CreateBuffer(ID3D12Device* device);

	//--- 内部変数 ---

	//座標変換リソース
	ComPtr<ID3D12Resource> transformationBuffer_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//パーツへの参照
	std::unique_ptr<Mesh> mesh_ = nullptr;
	std::unique_ptr<Material> material_ = nullptr;
};
