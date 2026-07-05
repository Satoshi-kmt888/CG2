#include "Mesh.h"

#include <dxgiformat.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <numbers>
#include <sstream>

#include "Graphics/D3D12Util.h"
#include "Graphics/DirectXCommon.h"

std::unique_ptr<Mesh> Mesh::CreateQuad() {
	std::unique_ptr<Mesh> mesh(new Mesh());

	mesh->vertices_ = {
		{ {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }, // 左上
		{ {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }, // 右上
		{ {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }, // 左下
		{ {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }, // 右下
	};

	mesh->indices_ = {
		0, 1, 2,
		1, 3, 2
	};

	mesh->CreateBuffers();

	return mesh;
}

std::unique_ptr<Mesh> Mesh::CreateSphere(uint32_t divisionVertical, uint32_t divisionHorizontal) {
	std::unique_ptr<Mesh> mesh(new Mesh());

	const float kLonEvery = 2.0f * std::numbers::pi_v<float> / static_cast<float>(divisionHorizontal);
	const float kLatEvery = std::numbers::pi_v<float> / static_cast<float>(divisionVertical);

	//頂点データの作成
	for (uint32_t latIndex = 0; latIndex <= divisionVertical; ++latIndex) {
		//緯度の方向に分割
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex <= divisionHorizontal; ++lonIndex) {
			//軽度の方向に分割
			float lon = lonIndex * kLonEvery;

			VertexData vertex{};
			//座標計算
			vertex.position.x = std::cos(lat) * std::cos(lon);
			vertex.position.y = std::sin(lat);
			vertex.position.z = std::cos(lat) * std::sin(lon);
			vertex.position.w = 1.0f;

			//UV座標
			vertex.texCoord.x = static_cast<float>(lonIndex) / divisionHorizontal;
			vertex.texCoord.y = 1.0f - static_cast<float>(latIndex) / divisionVertical;

			//法線
			vertex.normal = { vertex.position.x, vertex.position.y, vertex.position.z };

			//配列の末尾にデータを入れる
			mesh->vertices_.push_back(vertex);
		}
	}

	// インデックスデータの生成 (四角形を2つの三角形に分割)
	for (uint32_t latIndex = 0; latIndex < divisionVertical; ++latIndex) {
		for (uint32_t lonIndex = 0; lonIndex < divisionHorizontal; ++lonIndex) {
			//格子の左下の頂点番号を算出
			uint32_t start = latIndex * (divisionHorizontal + 1) + lonIndex;

			//1つ目の三角形(左下->左上->右上)
			mesh->indices_.push_back(start);
			mesh->indices_.push_back(start + (divisionHorizontal + 1));
			mesh->indices_.push_back(start + 1);

			//2つ目の三角形(右上->左上->右下)
			mesh->indices_.push_back(start + 1);
			mesh->indices_.push_back(start + (divisionHorizontal + 1));
			mesh->indices_.push_back(start + (divisionHorizontal + 1) + 1);
		}
	}

	mesh->CreateBuffers();

	return mesh;
}

std::unique_ptr<Mesh> Mesh::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	//必要な変数を宣言
	std::unique_ptr<Mesh> mesh(new Mesh());
	std::vector<Vector4> positions; //位置
	std::vector<Vector3> normals; //法線
	std::vector<Vector2> texCoords; //uv座標
	std::string line; //ファイルから読み込んだ1行を格納
	std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
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
			mesh->material_ = Material::LoadMaterialTemplateFile(directoryPath, materialFilename);
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
			VertexData triangle[3]{};
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
			mesh->vertices_.push_back(triangle[2]);
			mesh->vertices_.push_back(triangle[1]);
			mesh->vertices_.push_back(triangle[0]);
		}
	}

	mesh->CreateBuffers();

	return mesh;
}

void Mesh::Bind(ID3D12GraphicsCommandList* commandList) const {
	//トポロジをセット(三角形をセット)
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点バッファのセット
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//インデックスバッファのセット(インデックスがある場合)
	if (!indices_.empty()) {
		commandList->IASetIndexBuffer(&indexBufferView_);
	}
}

void Mesh::CreateBuffers() {
	auto device = DirectXCommon::GetInstance()->GetDevice();

	//頂点バッファの作成
	size_t vertexSize = sizeof(VertexData) * vertices_.size();
	vertexResource_ = CreateBufferResource(device, vertexSize);

	//頂点バッファの転送
	void* vertexPtr = nullptr;
	vertexResource_->Map(0, nullptr, &vertexPtr);
	std::memcpy(vertexPtr, vertices_.data(), vertexSize);
	vertexResource_->Unmap(0, nullptr);

	//ビューの設定
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast<UINT>(vertexSize);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//インデックスバッファの作成と転送(インデックスが存在する場合)
	if (!indices_.empty()) {
		//インデックスバッファの作成
		size_t indexSize = sizeof(uint32_t) * indices_.size();
		indexResource_ = CreateBufferResource(device, indexSize);

		//インデックスバッファの転送
		void* indexPtr = nullptr;
		indexResource_->Map(0, nullptr, &indexPtr);
		std::memcpy(indexPtr, indices_.data(), indexSize);
		indexResource_->Unmap(0, nullptr);

		//ビューの設定
		indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
		indexBufferView_.SizeInBytes = static_cast<UINT>(indexSize);
		indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	}
}
