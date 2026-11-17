#include "Sprite.h"

#include "Debugger/Logger.h"
#include "Graphics/D3D12Utility.h"
#include "Graphics/GraphicsSystem.h"

Sprite::~Sprite() = default;

std::unique_ptr<Sprite> Sprite::Create(const std::string& filename) {
	//デバイスを取得
	ID3D12Device* device = GraphicsSystem::GetInstance()->GetDevice();

	//各インスタンスを生成
	auto sprite = std::make_unique<Sprite>();
	sprite->m_material = std::make_unique<Material>();
	sprite->m_mesh = std::make_unique<Mesh>();

	float left = 0.0f - sprite->m_anchor_point.x;
	float top = 0.0f - sprite->m_anchor_point.y;
	float right = 1.0f - sprite->m_anchor_point.x;
	float bottom = 1.0f - sprite->m_anchor_point.y;

	std::array<Mesh::VertexData, 4> vertices = { {
		{ { left, top, 0.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, -1.0f } }, // 左上
		{ { right, top, 0.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, -1.0f } }, // 右上
		{ { left, bottom, 0.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, -1.0f } }, // 左下
		{ { right, bottom, 0.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, -1.0f } }, // 右下
	} };

	for (const auto& vertex : vertices) {
		sprite->m_mesh->AddVertex(vertex);
	}

	//インデックスデータを生成
	sprite->m_mesh->AddIndex(0);
	sprite->m_mesh->AddIndex(1);
	sprite->m_mesh->AddIndex(2);
	sprite->m_mesh->AddIndex(2);
	sprite->m_mesh->AddIndex(1);
	sprite->m_mesh->AddIndex(3);

	sprite->m_mesh->CreateBuffer(device);

	//マテリアルにテクスチャをセットしてバッファを作る
	sprite->m_material->SetTexture("Resource/" + filename);
	if (!sprite->m_material->Initialize(device)) {
		return nullptr;
	}
	sprite->m_material->SetLightType(0); //ライティングはオフ

	//スプライト本体を作る
	sprite->m_buffer.Create(device);

	return sprite;
}

void Sprite::Draw() {
	auto command_list = GraphicsSystem::GetInstance()->GetCommandList();

	//ワールド変換データを更新
	m_world_transform.scale = { m_size.x, m_size.y, 0.0f };
	m_world_transform.rotation = { 0.0f, 0.0f, m_rotation }; //Rollだけにする
	m_world_transform.translation = { m_position.x, m_position.y, 0.0f };

	Matrix4x4 mat_anchor = Matrix4x4::MakeTranslation({ -m_anchor_point.x, -m_anchor_point.y, 0.0f });
	Matrix4x4 mat_world = Matrix4x4::MakeAffine(m_world_transform.scale, m_world_transform.rotation, m_world_transform.translation);
	Matrix4x4 mat_projection = Matrix4x4::MakeOrthographic(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);
	m_buffer->world = mat_anchor * mat_world;
	m_buffer->wvp = m_buffer->world * mat_projection;
	command_list->SetGraphicsRootConstantBufferView(1, m_buffer.GetGPUAddress());

	//マテリアルを更新
	m_material->Update();

	//コマンド実行
	m_material->SetGraphicsCommand(command_list, 0, 2);
	m_mesh->Draw(command_list);
}
