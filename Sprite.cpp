#include "Sprite.h"

#include "DirectXCommon.h"

void Sprite::Initialize(ID3D12DescriptorHeap* descriptorHeap) {
	CreateVertexBuffer(6);

	//1枚目の三角形
	vertexData[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };//左下
	vertexData[0].texCoord = { 0.0f, 1.0f };
	vertexData[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };//左上
	vertexData[1].texCoord = { 0.0f, 0.0f };
	vertexData[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };//右下
	vertexData[2].texCoord = { 1.0f, 1.0f };

	//2枚目の三角形
	vertexData[3].position = { 0.0f, 0.0f, 0.0f, 1.0f };//左上
	vertexData[3].texCoord = { 0.0f, 0.0f };
	vertexData[4].position = { 640.0f, 0.0f, 0.0f, 1.0f };//右上
	vertexData[4].texCoord = { 1.0f, 0.0f };
	vertexData[5].position = { 640.0f, 360.0f, 0.0f, 1.0f };//右下
	vertexData[5].texCoord = { 1.0f, 1.0f };

	CreateWVPBuffer();

	transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
}

void Sprite::Update(const Matrix4x4& viewProjectionMatrix) {
	//ワールド行列更新
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

	Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
	*wvpData = worldViewProjectionMatrix;
}

void Sprite::Draw() {
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//形状を設定。PSOとは別途設定。同じものを設定
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//wvp用のCBufferの場所を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	//描画(DrawCall)
	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(6, 1, 0, 0);
}
