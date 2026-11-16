#pragma once

#include "3d/Material.h"
#include "3d/Mesh.h"
#include "Graphics/ConstantBuffer.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
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
	/// 通常のスプライトを生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Sprite> Create(const std::string& filename);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="viewProjectionMatrix"></param>
	void Draw();

	//=========================================================
	// accessor
	//=========================================================

	const Vector4& GetColor() const { return m_material->GetColor(); }
	uint32_t GetLightType() const { return m_material->GetLightType(); }
	const WorldTransform& GetUVTransform() const { return m_material->GetTransform(); }

	//---------------------------------------------------------

	void SetPosition(const Vector2& position) { m_position = position; }
	void SetRotation(float rotation) { m_rotation = rotation; }
	void SetSize(const Vector2& size) { m_size = size; }
	void SetColor(const Vector4& color) { m_material->SetColor(color); }

	void SetUVTransform(const WorldTransform& transform) { m_material->SetTransform(transform); }

private:
	//=========================================================
	// private methods
	//=========================================================

	//=========================================================
	// private variables
	//=========================================================

	ConstantBuffer<TransformationMatrix> m_buffer;

	//パーツへの参照
	std::unique_ptr<Mesh> m_mesh = nullptr;
	std::unique_ptr<Material> m_material = nullptr;

	WorldTransform m_world_transform{};

	Vector2 m_position{};
	float m_rotation{};
	Vector2 m_size{};
	Vector2 m_anchor_point{};
	Vector4 m_color{};
};
