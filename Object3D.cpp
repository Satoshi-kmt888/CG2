#include "Object3D.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"
#include "Matrix4x4.h"

#include <d3d12.h>

Object3D::~Object3D() {
	//WVPリソース
	if (transformationResource_) {
		transformationResource_->Unmap(0, nullptr);
	}
}

void Object3D::Initialize() {
	transformationResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	transformationData_->WVP = Matrix4x4::Identity();
	transformationData_->World = Matrix4x4::Identity();

	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f} };
}

void Object3D::Update(const Matrix4x4& viewProjectionMatrix) {
	transformationData_->World = Matrix4x4::MakeAffineMatrix(transform_.scale, transform_.rotation, transform_.translation);

	transformationData_->WVP = transformationData_->World * viewProjectionMatrix;
}

void Object3D::Draw() {
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootConstantBufferView(0, model_->GetMaterialResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());

	if (model_) {
		

		model_->Draw(commandList);
	}
}
