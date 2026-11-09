#pragma once

#include "3d/Material.h"
#include "3d/Mesh.h"
#include "3d/Model.h"

#include <d3d12.h>
#include <memory>
#include <span>

/// <summary>
/// 
/// </summary>
class Particle {
public:
	struct TransformationMatrix {
		Matrix4x4 wvp;
		Matrix4x4 world;
	};

	//=========================================================
	// public methods
	//=========================================================

	Particle();
	~Particle();

	void Initialize(UINT instance_count);
	void Update();
	void Draw(std::span<const Transform> transforms, const Matrix4x4& view_projection_matrix);

	ID3D12Resource* GetResource() const { return m_transformation_buffer.Get(); }

private:
	//=========================================================
	// private methods
	//=========================================================

	void CreateBuffer(ID3D12Device* device);

	//=========================================================
	// private variables
	//=========================================================

	UINT m_instance_count = 0;

	//座標変換リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> m_transformation_buffer = nullptr;
	TransformationMatrix* m_transformation_data = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE m_instancing_srv_gpu_handle{};

	std::unique_ptr<Model> m_model;
	std::unique_ptr<Mesh> m_mesh;
	std::unique_ptr<Material> m_material;
};

