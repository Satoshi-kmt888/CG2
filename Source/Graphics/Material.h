#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector4.h"

#include <array>
#include <cstdint>
#include <d3d12.h>
#include <Windows.h>
#include <wrl/client.h>
#include <string>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct TextureData;

/// <summary>
/// 物体の質感、色、テクスチャを管理するクラス
/// </summary>
class Material {
public:
	//--- 内部データ構造体 ---

	//GPUへ送るための表面データ
	struct SurfaceData {
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; //RGBAの色データ
		int32_t enableLighting = 1; //ライティング有効フラグ
		std::array<float, 3> padding;       //パディング
		Matrix4x4 uvTransform = Matrix4x4::Identity();  //uv座標変換データ
	};

	//--- コンストラクタ・デストラクタ ---

	Material() = default;
	~Material();

	//--- 公開関数 ---

	/// <summary>
	/// 
	/// </summary>
	/// <param name="device"></param>
	void Build(ID3D12Device* device);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// コマンドリストとの紐づけ
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="rootParamIndexMaterial"></param>
	/// <param name="rootParamIndexTexture"></param>
	void Bind(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial, UINT rootParamIndexTexture);

	void SetTexture(const std::string& filePath);

private:
	//--- 内部変数 ---

	ComPtr<ID3D12Resource> surfaceBuffer_ = nullptr;
	SurfaceData* surfaceData_ = nullptr;
	std::string textureFilePath;
	const TextureData* textureData_ = nullptr;
};
