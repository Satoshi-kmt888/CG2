#include "Object3D.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"
#include "Matrix4x4.h"
#include "Vector4.h"

#include <d3d12.h>
#include <d3dcommon.h>

void Object3D::Initialize(ID3D12DescriptorHeap* descriptorHeap) {
	//SRVを作成するDescriptorHeapの場所を決める
	textureSrvHandleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	//先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//頂点リソースの作成
	CreateVertexBuffer(6);

	//--- 1枚目 ---
	//左下
	vertexData[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
	vertexData[0].texCoord = { 0.0f, 1.0f };
	//上
	vertexData[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
	vertexData[1].texCoord = { 0.5f, 0.0f };
	//右下
	vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
	vertexData[2].texCoord = { 1.0f, 1.0f };

	//--- 2枚目 ---
	//左下
	vertexData[3].position = { -0.5f, -0.5f, 0.5f, 1.0f };
	vertexData[3].texCoord = { 0.0f, 1.0f };
	//上
	vertexData[4].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexData[4].texCoord = { 0.5f, 0.0f };
	//右下
	vertexData[5].position = { 0.5f, -0.5f, -0.5f, 1.0f };
	vertexData[5].texCoord = { 1.0f, 1.0f };

	//マテリアルリソースの作成
	CreateMaterialBuffer();

	//WVPリソースの作成
	CreateWVPBuffer();

	/*Transformの初期化*/
	transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
}

void Object3D::Update(const Matrix4x4& viewProjectionMatrix) {
	//回転
	transform.rotation.y += 0.005f;

	//ワールド行列更新
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

	Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
	*wvpData = worldViewProjectionMatrix;
}

void Object3D::Draw() {
	RecordCommonCommand(DirectXCommon::GetInstance()->GetCommandList());
	//形状を設定。PSOとは別途設定。同じものを設定
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//描画(DrawCall)
	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(6, 1, 0, 0);
}
