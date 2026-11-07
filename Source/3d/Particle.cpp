#include "Particle.h"

#include "Debugger/Logger.h"
#include "Graphics/D3D12Utility.h"
#include "Graphics/GraphicsSystem.h"

#include <algorithm>

Particle::Particle() = default;

Particle::~Particle() = default;

void Particle::Initialize(UINT instance_count) {
	m_instance_count = instance_count;

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

	m_material->SetTexture("Resource/uvChecker.png");
	m_material->CreateBuffer(device);

	CreateBuffer(device);
}

void Particle::Update() {

}

void Particle::Draw(std::span<const Transform> transforms, const Matrix4x4& view_projection_matrix) {
	auto commandList = GraphicsSystem::GetInstance()->GetCommandList();

	if (!m_transformation_buffer) {
		LOG_ERROR("Model::Draw メンバ変数transformationBuffer_がnullptrです。Model::CreateBufferでtransformationBuffer_を作成してください。");
		return;
	}

	const size_t count = std::min<size_t>(transforms.size(), m_instance_count);

	//ワールド変換データを更新
	for (uint32_t i = 0; i < m_instance_count; ++i) {
		const Matrix4x4 mat_world = transforms[i].UpdateMatrix();
		m_transformation_data[i].world = mat_world;
		m_transformation_data[i].wvp = mat_world * view_projection_matrix;
	}

	//マテリアルを更新
	m_material->Update();

	//コマンド実行
	m_material->SetGraphicsCommand(commandList, 0, 2);
	m_mesh->Draw(commandList, m_instance_count);
}

void Particle::CreateBuffer(ID3D12Device* device) {
	m_transformation_buffer = D3D12Utility::CreateBufferResource(device, sizeof(TransformationMatrix) * m_instance_count);
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

	m_transformation_data = static_cast<TransformationMatrix*>(transformation_ptr);
	for (uint32_t i = 0; i < m_instance_count; ++i) {
		if (m_transformation_data) {
			m_transformation_data[i].wvp = Matrix4x4::Identity();
			m_transformation_data[i].world = Matrix4x4::Identity();
		}
	}
}
