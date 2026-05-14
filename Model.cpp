#include "Model.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"
#include "Mesh.h"

#include <Windows.h>

void Model::Initialize() {
	//座標変換用の定数バッファ作成
	transformationResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));

	//単位行列で初期化
	transformationData_->WVP = Matrix4x4::Identity();
	transformationData_->World = Matrix4x4::Identity();

	//Materialの生成・初期化
	material_ = std::make_unique<Material>();
	material_->Initialize();

	//デフォルト値を設定
	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f} };
}

void Model::Update(const Matrix4x4& viewProjectionMatrix) {
	transformationData_->World = Matrix4x4::MakeAffineMatrix(transform_.scale, transform_.rotation, transform_.translation);

	transformationData_->WVP = transformationData_->World * viewProjectionMatrix;
}

void Model::Draw() {
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	//メッシュ(形状)をセット
	mesh_->Bind(commandList);

	//マテリアル(素材)をセット
	material_->Bind(commandList, 0, 2);

	commandList->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());

	if (mesh_->GetIndexCount() > 0) {
		commandList->DrawIndexedInstanced(static_cast<UINT>(mesh_->GetIndexCount()), 1, 0, 0, 0);
	} else {
		commandList->DrawInstanced(static_cast<UINT>(mesh_->GetVertexCount()), 1, 0, 0);
	}
}
