#pragma once

#include "3d/Material.h"
#include "3d/Mesh.h"
#include "3d/Model.h"
#include "3d/Particle.h"
#include "Math/Vector4.h"

#include <d3d12.h>
#include <list>
#include <memory>

class Camera;

/// <summary>
/// 
/// </summary>
class ParticleSystem {
public:
	struct ParticleForGPU {
		Matrix4x4 wvp;
		Matrix4x4 world;
		Vector4 color;
	};

	//=========================================================
	// public methods
	//=========================================================

	ParticleSystem();
	~ParticleSystem();

	void Initialize();
	void Update();
	void Draw(const Camera* camera);

	ID3D12Resource* GetResource() const { return m_transformation_buffer.Get(); }

	void SetParticle(const std::list<Particle>& particle) { m_particles = particle; }

private:
	//=========================================================
	// private methods
	//=========================================================

	void CreateBuffer(ID3D12Device* device);

	//=========================================================
	// private variables
	//=========================================================

	const uint32_t k_num_max_instance = 1024;

	//座標変換リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> m_transformation_buffer = nullptr;
	ParticleForGPU* m_transformation_data = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE m_instancing_srv_gpu_handle{};

	std::unique_ptr<Model> m_model;
	std::unique_ptr<Mesh> m_mesh;
	std::unique_ptr<Material> m_material;

	std::list<Particle> m_particles;
};
