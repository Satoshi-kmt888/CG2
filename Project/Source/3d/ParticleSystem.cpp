#include "ParticleSystem.h"

#include "Camera.h"
#include "Debugger/Logger.h"
#include "Graphics/D3D12Utility.h"
#include "Graphics/GraphicsSystem.h"

#include <algorithm>

ParticleSystem::ParticleSystem() = default;

ParticleSystem::~ParticleSystem() = default;

void ParticleSystem::Initialize(UINT instance_count) {
	ID3D12Device* device = GraphicsSystem::GetInstance()->GetDevice();

	m_model = std::make_unique<Model>();
	m_material = std::make_unique<Material>();
	m_mesh = std::make_unique<Mesh>();

	//頂点データを生成
	Mesh::VertexData vertex;
	vertex = { { -0.5f, 0.5f, 0.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };   // 左上
	m_mesh->AddVertex(vertex);
	vertex = { { 0.5f, 0.5f, 0.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };  // 右上
	m_mesh->AddVertex(vertex);
	vertex = { { -0.5f, -0.5f, 0.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } };  // 左下
	m_mesh->AddVertex(vertex);
	vertex = { { 0.5f, -0.5f, 0.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } }; // 右下
	m_mesh->AddVertex(vertex);

	//インデックスデータを生成
	m_mesh->AddIndex(0);
	m_mesh->AddIndex(1);
	m_mesh->AddIndex(2);
	m_mesh->AddIndex(2);
	m_mesh->AddIndex(1);
	m_mesh->AddIndex(3);

	m_mesh->CreateBuffer(device);

	m_material->SetTexture("Resource/circle.png");
	m_material->CreateBuffer(device);

	CreateBuffer(device);

	m_particles.reserve(instance_count);

	m_transformation_data->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };


	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srv_desc.Buffer.NumElements = m_num_max_instance;
	srv_desc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	const DescriptorHandle allocation = DescriptorManager::GetInstance()->Allocate(DescriptorType::SRV_CBV_UAV);
	m_instancing_srv_gpu_handle = allocation.gpuHandle;
	device->CreateShaderResourceView(m_transformation_buffer.Get(), &srv_desc, allocation.cpuHandle);
}

void ParticleSystem::Update() {
}

void ParticleSystem::Draw(const Camera* camera) {
	auto command_list = GraphicsSystem::GetInstance()->GetCommandList();

	if (!m_transformation_buffer) {
		LOG_ERROR("Model::Draw メンバ変数transformationBuffer_がnullptrです。Model::CreateBufferでtransformationBuffer_を作成してください。");
		return;
	}

	const size_t count = std::min<size_t>(m_particles.size(), m_num_max_instance);
	uint32_t num_instance = 0;

	Matrix4x4 mat_billboard = camera->GetWorldMatrix();
	mat_billboard.m[3][0] = 0.0f;
	mat_billboard.m[3][1] = 0.0f;
	mat_billboard.m[3][2] = 0.0f;

	//ワールド変換データを更新
	for (auto& particle:m_particles) {
		if (num_instance >= m_num_max_instance) {
			break; //バッファの上限
		}

		if (particle.life_time <= particle.current_time) {
			continue;
		}

		float alpha = 1.0f - (particle.current_time / particle.life_time);

		Matrix4x4 mat_scale = Matrix4x4::MakeScale(particle.transform.scale);
		Matrix4x4 mat_translation = Matrix4x4::MakeTranslation(particle.transform.translation);

		particle.transform.mat_world = mat_scale * mat_billboard * mat_translation;
		m_transformation_data[num_instance].world = particle.transform.mat_world;
		m_transformation_data[num_instance].wvp = particle.transform.mat_world * camera->GetViewProjMatrix();

		m_transformation_data[num_instance].color = particle.color;
		m_transformation_data[num_instance].color.w = alpha;

		++num_instance;
	}

	if (num_instance == 0) {
		return;
	}

	command_list->SetGraphicsRootDescriptorTable(2, m_instancing_srv_gpu_handle);

	//マテリアルを更新
	m_material->Update();

	//コマンド実行
	m_material->SetGraphicsCommand(command_list, 0, 1);
	m_mesh->Draw(command_list, num_instance);
}

void ParticleSystem::CreateBuffer(ID3D12Device* device) {
	m_transformation_buffer = D3D12Utility::CreateBufferResource(device, sizeof(ParticleForGPU) * m_num_max_instance);
	if (!m_transformation_buffer) {
		LOG_ERROR("Model::CreateBuffer メンバ変数transformationBuffer_がnullptrです。Model::CreateBufferでtransformationBuffer_を作成してください。");
		return;
	}

	void* transformation_ptr = nullptr;
	HRESULT hr = m_transformation_buffer->Map(0, nullptr, &transformation_ptr);
	if (FAILED(hr)) {
		LOG_ERROR("Model::CreateBuffer メンバ変数transformationBufferをマッピングすることができませんでした。");
		return;
	}

	m_transformation_data = static_cast<ParticleForGPU*>(transformation_ptr);
	for (uint32_t i = 0; i < m_num_max_instance; ++i) {
		if (m_transformation_data) {
			m_transformation_data[i].wvp = Matrix4x4::Identity();
			m_transformation_data[i].world = Matrix4x4::Identity();
		}
	}
}
