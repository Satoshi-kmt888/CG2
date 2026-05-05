#include "Object3D.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"
#include "Matrix4x4.h"
#include "Vector4.h"

#include <d3d12.h>

Object3D::Object3D(){
}

Object3D::~Object3D(){
	// マテリアルリソースの Unmap
	if (materialResource_) {
		materialResource_->Unmap(0, nullptr);
	}

	// WVPリソースの Unmap
	if (wvpResource_) {
		wvpResource_->Unmap(0, nullptr);
	}
}

void Object3D::Initialize() {
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Vector4));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	*materialData_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	wvpResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = Matrix4x4::Identity();

	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f} };
}

void Object3D::Update(const Matrix4x4& viewProjectionMatrix) {
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform_.scale, transform_.rotation, transform_.translation);

	*wvpData_ = worldMatrix * viewProjectionMatrix;
}

void Object3D::Draw() {
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	if (model_) {
		model_->Draw(commandList);
	}
}
















//void Object3D::Initialize(ID3D12DescriptorHeap* descriptorHeap) {
//	
//
//	//--- 1枚目 ---
//	//左下
//	vertexData[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
//	vertexData[0].texCoord = { 0.0f, 1.0f };
//	//上
//	vertexData[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
//	vertexData[1].texCoord = { 0.5f, 0.0f };
//	//右下
//	vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
//	vertexData[2].texCoord = { 1.0f, 1.0f };
//
//	//--- 2枚目 ---
//	//左下
//	vertexData[3].position = { -0.5f, -0.5f, 0.5f, 1.0f };
//	vertexData[3].texCoord = { 0.0f, 1.0f };
//	//上
//	vertexData[4].position = { 0.0f, 0.0f, 0.0f, 1.0f };
//	vertexData[4].texCoord = { 0.5f, 0.0f };
//	//右下
//	vertexData[5].position = { 0.5f, -0.5f, -0.5f, 1.0f };
//	vertexData[5].texCoord = { 1.0f, 1.0f };
//
//	/*Transformの初期化*/
//	transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
//}
