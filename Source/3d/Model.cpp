#include "Model.h"

#include "Debugger/Logger.h"
#include "Graphics/D3D12Utility.h"
#include "Graphics/GraphicsSystem.h"
#include "Material.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Mesh.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <d3d12.h>
#include <fstream>
#include <memory>
#include <numbers>
#include <sstream>
#include <string>
#include <vector>
#include <Windows.h>

Model::~Model() {
	if (m_transformation_buffer && m_transformation_data) {
		m_transformation_buffer->Unmap(0, nullptr);
		m_transformation_data = nullptr;
	}
}

std::unique_ptr<Model> Model::CreatePlane() {
	ID3D12Device* device = GraphicsSystem::GetInstance()->GetDevice();

	auto model = std::make_unique<Model>();
	model->m_material = std::make_unique<Material>();
	model->m_mesh = std::make_unique<Mesh>();

	//頂点データを生成
	Mesh::VertexData vertex;
	vertex = { { -0.5f, 0.5f, 0.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };   // 左上
	model->m_mesh->AddVertex(vertex);
	vertex = { { 0.5f, 0.5f, 0.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };  // 右上
	model->m_mesh->AddVertex(vertex);
	vertex = { { -0.5f, -0.5f, 0.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } };  // 左下
	model->m_mesh->AddVertex(vertex);
	vertex = { { 0.5f, -0.5f, 0.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } }; // 右下
	model->m_mesh->AddVertex(vertex);

	//インデックスデータを生成
	model->m_mesh->AddIndex(0);
	model->m_mesh->AddIndex(1);
	model->m_mesh->AddIndex(2);
	model->m_mesh->AddIndex(2);
	model->m_mesh->AddIndex(1);
	model->m_mesh->AddIndex(3);

	model->m_mesh->CreateBuffer(device);

	model->m_material->SetTexture("Resource/uvChecker.png");
	model->m_material->CreateBuffer(device);

	model->CreateBuffer(device);

	return model;
}

std::unique_ptr<Model> Model::CreateSphere(uint32_t divisionHorizontal, uint32_t divisionVertical) {
	ID3D12Device* device = GraphicsSystem::GetInstance()->GetDevice();

	auto model = std::make_unique<Model>();
	model->m_material = std::make_unique<Material>();
	model->m_mesh = std::make_unique<Mesh>();

	const float k_lon_every = 2.0f * std::numbers::pi_v<float> / static_cast<float>(divisionHorizontal);
	const float k_lat_every = std::numbers::pi_v<float> / static_cast<float>(divisionVertical);

	//頂点データの作成
	for (uint32_t lat_index = 0; lat_index <= divisionVertical; ++lat_index) {
		//緯度の方向に分割
		float lat = -std::numbers::pi_v<float> / 2.0f + k_lat_every * static_cast<float>(lat_index);
		for (uint32_t lonIndex = 0; lonIndex <= divisionHorizontal; ++lonIndex) {
			//経度の方向に分割
			float lon = static_cast<float>(lonIndex) * k_lon_every;

			Mesh::VertexData vertex{};
			//座標計算
			vertex.position.x = std::cos(lat) * std::cos(lon);
			vertex.position.y = std::sin(lat);
			vertex.position.z = std::cos(lat) * std::sin(lon);
			vertex.position.w = 1.0f;

			//UV座標
			vertex.texcoord.x = static_cast<float>(lonIndex) / static_cast<float>(divisionHorizontal);
			vertex.texcoord.y = 1.0f - (static_cast<float>(lat_index) / static_cast<float>(divisionVertical));

			//法線
			vertex.normal = { vertex.position.x, vertex.position.y, vertex.position.z };

			//配列の末尾にデータを入れる
			model->m_mesh->AddVertex(vertex);
		}
	}

	// インデックスデータの生成(四角形を2つの三角形に分割)
	for (uint32_t lat_index = 0; lat_index < divisionVertical; ++lat_index) {
		for (uint32_t lonIndex = 0; lonIndex < divisionHorizontal; ++lonIndex) {
			//格子の左下の頂点番号を算出
			uint32_t start = lat_index * (divisionHorizontal + 1) + lonIndex;

			// 1つ目の三角形 (左下->左上->右上)
			model->m_mesh->AddIndex(start);
			model->m_mesh->AddIndex(start + (divisionHorizontal + 1));
			model->m_mesh->AddIndex(start + 1);

			// 2つ目の三角形 (右上->左上->右下)
			model->m_mesh->AddIndex(start + 1);
			model->m_mesh->AddIndex(start + (divisionHorizontal + 1));
			model->m_mesh->AddIndex(start + (divisionHorizontal + 1) + 1);
		}
	}

	model->m_mesh->CreateBuffer(device);

	model->m_material->SetTexture("Resource/uvChecker.png");
	model->m_material->CreateBuffer(device);

	model->CreateBuffer(device);

	return model;
}

std::unique_ptr<Model> Model::CreateFromOBJ(const std::string& filename) {
	ID3D12Device* device = GraphicsSystem::GetInstance()->GetDevice();
	if (!device) {
		LOG_ERROR("Model::CreateFromOBJ -> ID3D12Deviceがnullptrです。");
		return nullptr;
	}

	auto model = std::make_unique<Model>();
	model->m_material = std::make_unique<Material>();
	model->m_mesh = std::make_unique<Mesh>();

	model->LoadOBJ(filename);

	model->m_material->CreateBuffer(device);

	model->CreateBuffer(device);

	return model;
}

void Model::Draw(const Transform& transform, const Matrix4x4& view_projection_matrix) {
	auto commandList = GraphicsSystem::GetInstance()->GetCommandList();

	if (!m_transformation_buffer) {
		LOG_ERROR("Model::Draw メンバ変数transformationBuffer_がnullptrです。Model::CreateBufferでtransformationBuffer_を作成してください。");
		return;
	}

	//ワールド変換データを更新
	m_transformation_data->world = Transform::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);
	m_transformation_data->wvp = m_transformation_data->world * view_projection_matrix;

	//マテリアルを更新
	m_material->Update();

	commandList->SetGraphicsRootConstantBufferView(1, m_transformation_buffer->GetGPUVirtualAddress());

	//コマンド実行
	m_material->SetGraphicsCommand(commandList, 0, 2);
	m_mesh->Draw(commandList);
}

void Model::CreateBuffer(ID3D12Device* device) {
	m_transformation_buffer = D3D12Utility::CreateBufferResource(device, sizeof(TransformationMatrix));
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
	if (m_transformation_data) {
		m_transformation_data->wvp = Matrix4x4::Identity();
		m_transformation_data->world = Matrix4x4::Identity();
	}
}

void Model::LoadOBJ(const std::string& filename) {
	//必要な変数を宣言
	std::vector<Vector4> positions; //位置
	std::vector<Vector3> normals; //法線
	std::vector<Vector2> texcoords; //uv座標
	std::string line; //ファイルから読み込んだ1行を格納
	std::ifstream file("Resource/" + filename); //ファイルを開く
	assert(file.is_open()); //開かなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; //先頭の識別を読む

		//identifierに応じた処理
		if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;

			//基本的にobjファイルと同じ改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			LoadMaterialTemplateFile(materialFilename);
		} else if (identifier == "v") {
			Vector4 position{};
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texCoord{};
			s >> texCoord.x >> texCoord.y;
			texcoords.push_back(texCoord);
		} else if (identifier == "vn") {
			Vector3 normal{};
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		} else if (identifier == "f") {
			Mesh::VertexData triangle[3]{};
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3]{};
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}

				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[static_cast<size_t>(elementIndices[0]) - 1];
				Vector2 texCoord = texcoords[static_cast<size_t>(elementIndices[1]) - 1];
				Vector3 normal = normals[static_cast<size_t>(elementIndices[2]) - 1];
				position.x *= -1.0f;
				texCoord.y = 1.0f - texCoord.y;
				normal.x *= -1.0f;
				//VertexData vertex = { position, texCoord, normal };
				//mesh->vertices_.push_back(vertex);
				triangle[faceVertex] = { position, texCoord, normal };
			}

			//頂点を逆順で登録することで、周り順を逆にする
			m_mesh->AddVertex(triangle[2]);
			m_mesh->AddVertex(triangle[1]);
			m_mesh->AddVertex(triangle[0]);
		}
	}

	m_mesh->CreateBuffer(GraphicsSystem::GetInstance()->GetDevice());
}

void Model::LoadMaterialTemplateFile(const std::string& filename) {
	//必要な変数を宣言
	std::string line; //ファイルから読み込んだ1行を格納
	std::ifstream file("Resource/" + filename); //ファイルを開く
	assert(file.is_open()); //開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			m_material->SetTexture("Resource/" + textureFilename);
		}
	}
}
