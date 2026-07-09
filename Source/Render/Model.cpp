#include "Render/Model.h"

#include "Graphics/D3D12Utility.h"
#include "Graphics/GraphicsSystem.h"
#include "Diagnostics/Logger.h"

#include <numbers>

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
			//軽度の方向に分割
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
	return std::unique_ptr<Model>();
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
