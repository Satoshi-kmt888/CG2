#pragma once

#include "Graphics/ConstantBuffer.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector4.h"
#include "Math/WorldTransform.h"

#include <array>
#include <cstdint>
#include <d3d12.h>
#include <string>
#include <Windows.h>
#include <wrl/client.h>

struct TextureData;

/// <summary>
/// 物体の質感、色、テクスチャを管理するクラス
/// </summary>
class Material {
public:
	//GPUへ送るための表面データ
	struct SurfaceData {
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; //RGBAの色データ
		int32_t lightType = 0; //ライティング有効フラグ
		std::array<float, 3> padding;       //パディング
		Matrix4x4 uvTransform = Matrix4x4::Identity();  //uv座標変換データ
	};

	//=========================================================
	// public methods
	//=========================================================

	Material() = default;
	~Material() = default;

	[[nodiscard]] bool Initialize(ID3D12Device* device);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// コマンドリスト
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="rootParamIndexMaterial"></param>
	/// <param name="rootParamIndexTexture"></param>
	void SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial, UINT rootParamIndexTexture) const;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="filePath"></param>
	void SetTexture(const std::string& filePath);

	//=========================================================
	// accessor
	//=========================================================

	const Vector4& GetColor() const { return m_buffer->color; }
	int32_t GetLightType() const { return m_buffer->lightType; }
	const WorldTransform& GetTransform() const { return transform_; }

	//---------------------------------------------------------

	void SetColor(const Vector4& color) { m_buffer->color = color; }
	void SetLightType(int32_t lightType) { m_buffer->lightType = lightType; }
	void SetTransform(const WorldTransform& transform) { transform_ = transform; }

private:
	//=========================================================
	// private methods
	//=========================================================

	ConstantBuffer<SurfaceData> m_buffer;

	const TextureData* textureData_ = nullptr;

	WorldTransform transform_ = {};
};
