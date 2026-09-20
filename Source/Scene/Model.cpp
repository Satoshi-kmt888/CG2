#include "Scene/Model.h"

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
	if (transformationBuffer_ && transformationData_) {
		transformationBuffer_->Unmap(0, nullptr);
		transformationData_ = nullptr;
	}
}

std::unique_ptr<Model> Model::CreateSphere(uint32_t divisionHorizontal, uint32_t divisionVertical) {
	ID3D12Device* device = GraphicsSystem::GetInstance()->GetDevice();

	auto model = std::make_unique<Model>();
	model->material_ = std::make_unique<Material>();
	model->mesh_ = std::make_unique<Mesh>();

	const float kLonEvery = 2.0f * std::numbers::pi_v<float> / static_cast<float>(divisionHorizontal);
	const float kLatEvery = std::numbers::pi_v<float> / static_cast<float>(divisionVertical);

	//頂点データの作成
	for (uint32_t latIndex = 0; latIndex <= divisionVertical; ++latIndex) {
		//緯度の方向に分割
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * static_cast<float>(latIndex);
		for (uint32_t lonIndex = 0; lonIndex <= divisionHorizontal; ++lonIndex) {
			//経度の方向に分割
			float lon = static_cast<float>(lonIndex) * kLonEvery;

			Mesh::VertexData vertex{};
			//座標計算
			vertex.position.x = std::cos(lat) * std::cos(lon);
			vertex.position.y = std::sin(lat);
			vertex.position.z = std::cos(lat) * std::sin(lon);
			vertex.position.w = 1.0f;

			//UV座標
			vertex.texCoord.x = static_cast<float>(lonIndex) / static_cast<float>(divisionHorizontal);
			vertex.texCoord.y = 1.0f - (static_cast<float>(latIndex) / static_cast<float>(divisionVertical));

			//法線
			vertex.normal = { vertex.position.x, vertex.position.y, vertex.position.z };

			//配列の末尾にデータを入れる
			model->mesh_->AddVertex(vertex);
		}
	}

	// インデックスデータの生成(四角形を2つの三角形に分割)
	for (uint32_t latIndex = 0; latIndex < divisionVertical; ++latIndex) {
		for (uint32_t lonIndex = 0; lonIndex < divisionHorizontal; ++lonIndex) {
			//格子の左下の頂点番号を算出
			uint32_t start = latIndex * (divisionHorizontal + 1) + lonIndex;

			// 1つ目の三角形 (左下->左上->右上)
			model->mesh_->AddIndex(start);
			model->mesh_->AddIndex(start + (divisionHorizontal + 1));
			model->mesh_->AddIndex(start + 1);

			// 2つ目の三角形 (右上->左上->右下)
			model->mesh_->AddIndex(start + 1);
			model->mesh_->AddIndex(start + (divisionHorizontal + 1));
			model->mesh_->AddIndex(start + (divisionHorizontal + 1) + 1);
		}
	}

	model->mesh_->CreateBuffer(device);

	model->material_->SetTexture("Resources/uvChecker.png");
	model->material_->CreateBuffer(device);

	model->CreateBuffer(device);

	return model;
}

std::unique_ptr<Model> Model::CreateFromOBJ(const std::string& filename) {
	// 1. ModelRenderSystem (コード内では GraphicsSystem) からデバイスを取得
	ID3D12Device* device = GraphicsSystem::GetInstance()->GetDevice();
	if (!device) {
		LOG_ERROR("Model::CreateFromOBJ -> ID3D12Deviceがnullptrです。");
		return nullptr;
	}

	// 2. インスタンスと、内包する各パーツの生成
	auto model = std::make_unique<Model>();
	model->material_ = std::make_unique<Material>();
	model->mesh_ = std::make_unique<Mesh>();

	// 3. OBJファイルの解析と頂点・インデックスデータの構築
	// (LoadOBJ内部で model->mesh_ へデータを追加していく)
	model->LoadOBJ(filename);

	// 4. マテリアル用の定数バッファを生成
	// ※現時点では LoadMaterialTemplateFile 内でのマテリアルバッファ作成や
	// テクスチャ設定がまだ仮実装のため、ここで一旦作成を行います。
	model->material_->CreateBuffer(device);

	// 5. モデル自身の座標変換用定数バッファを生成
	model->CreateBuffer(device);

	// 6. セットアップが完了したモデルを返す
	return model;
}

void Model::Draw(const Transform& transform, const Matrix4x4& viewProjectionMatrix) {
	auto commandList = GraphicsSystem::GetInstance()->GetCommandList();

	if (!transformationBuffer_) {
		LOG_ERROR("Model::Draw メンバ変数transformationBuffer_がnullptrです。Model::CreateBufferでtransformationBuffer_を作成してください。");
		return;
	}

	//ワールド変換データを更新
	transformationData_->world = Transform::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);
	transformationData_->wvp = transformationData_->world * viewProjectionMatrix;

	//マテリアルを更新
	material_->Update();

	commandList->SetGraphicsRootConstantBufferView(1, transformationBuffer_->GetGPUVirtualAddress());

	//コマンド実行
	material_->SetGraphicsCommand(commandList, 0, 2);
	mesh_->Draw(commandList);
}

void Model::CreateBuffer(ID3D12Device* device) {
	transformationBuffer_ = D3D12Utility::CreateBufferResource(device, sizeof(TransformationMatrix));
	if (!transformationBuffer_) {
		LOG_ERROR("Model::CreateBuffer メンバ変数transformationBuffer_がnullptrです。Model::CreateBufferでtransformationBuffer_を作成してください。");
		return;
	}

	void* transformationPtr = nullptr;
	HRESULT hr = transformationBuffer_->Map(0, nullptr, &transformationPtr);
	if (FAILED(hr)) {
		LOG_ERROR("Model::CreateBuffer メンバ変数transformationBufferをマッピングすることができませんでした。");
		return;
	}

	transformationData_ = static_cast<TransformationMatrix*>(transformationPtr);
	if (transformationData_) {
		transformationData_->wvp = Matrix4x4::Identity();
		transformationData_->world = Matrix4x4::Identity();
	}
}

void Model::LoadOBJ(const std::string& filename) {
	//必要な変数を宣言
	std::vector<Vector4> positions; //位置
	std::vector<Vector3> normals; //法線
	std::vector<Vector2> texCoords; //uv座標
	std::string line; //ファイルから読み込んだ1行を格納
	std::ifstream file("Resources/" + filename); //ファイルを開く
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
			texCoords.push_back(texCoord);
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
				Vector2 texCoord = texCoords[static_cast<size_t>(elementIndices[1]) - 1];
				Vector3 normal = normals[static_cast<size_t>(elementIndices[2]) - 1];
				position.x *= -1.0f;
				texCoord.y = 1.0f - texCoord.y;
				normal.x *= -1.0f;
				//VertexData vertex = { position, texCoord, normal };
				//mesh->vertices_.push_back(vertex);
				triangle[faceVertex] = { position, texCoord, normal };
			}

			//頂点を逆順で登録することで、周り順を逆にする
			mesh_->AddVertex(triangle[2]);
			mesh_->AddVertex(triangle[1]);
			mesh_->AddVertex(triangle[0]);
		}
	}

	mesh_->CreateBuffer(GraphicsSystem::GetInstance()->GetDevice());
}

void Model::LoadMaterialTemplateFile(const std::string& filename) {
	//必要な変数を宣言
	std::string line; //ファイルから読み込んだ1行を格納
	std::ifstream file("Resources/" + filename); //ファイルを開く
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
			material_->SetTexture("Resources/" + textureFilename);
		}
	}
}
