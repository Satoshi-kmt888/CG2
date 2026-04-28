#include "Sprite.h"

#include "DirectXCommon.h"
#include "D3D12Util.h"
#include "WinApp.h"

void Sprite::Initialize(ID3D12DescriptorHeap* descriptorHeap) {
	//Sprite用の頂点リソースを作る
	vertexResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * 6);

	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

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

	//Sprite用のTransformationMatrix用のリソースを作る
	wvpResource = CreateBufferResource(
		DirectXCommon::GetInstance()->GetDevice(),
		sizeof(Matrix4x4)
	);
	//データを書き込む
	wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでいく
	*wvpData = Matrix4x4::Identity();

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
