#include "Model.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"
#include "Mesh.h"

#include <Windows.h>

std::unique_ptr<Model> Model::CreateSphere(const std::string& textureFilePath) {
	auto model = std::make_unique<Model>();
	
	//メッシュセット
	model->mesh_ = std::make_unique<Mesh>();
	model->mesh_ = Mesh::CreateSphere();

	//マテリアルをセット
	model->material_ = std::make_unique<Material>();
	model->material_->Initialize();
	model->material_->SetTexture(textureFilePath);

	model->Initialize();

	return model;
}

void Model::Initialize() {
	//座標変換用の定数バッファ作成
	transformationResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));

	//単位行列で初期化
	transformationData_->WVP = Matrix4x4::Identity();
	transformationData_->World = Matrix4x4::Identity();

	//デフォルト値を設定
	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f} };
}

void Model::Update(const Matrix4x4& viewProjectionMatrix) {
	transformationData_->World = Matrix4x4::MakeAffineMatrix(transform_.scale, transform_.rotation, transform_.translation);

	transformationData_->WVP = transformationData_->World * viewProjectionMatrix;

	material_->Update();
}

void Model::Draw() {
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	//メッシュ(形状)をセット
	mesh_->Bind(commandList);

	//マテリアル(素材)をセット
	material_->Bind(commandList, 0, 2);

	commandList->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());

	if (!mesh_->GetIndices().empty()) {
		commandList->DrawIndexedInstanced(static_cast<UINT>(mesh_->GetIndices().size()), 1, 0, 0, 0);
	} else {
		commandList->DrawInstanced(static_cast<UINT>(mesh_->GetVertices().size()), 1, 0, 0);
	}
}
