#include "Object3D.h"
#include "D3D12Util.h"

void Object3D::Initialize(ID3D12Device* device) {
	/*頂点リソースの作成*/
	//データ書き込み
	vertexResource = CreateBufferResource(device, sizeof(Vector4) * 3);
	//リソースの先頭アドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(Vector4) * 3;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(Vector4);

	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	vertexData[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; //左下
	vertexData[1] = { 0.0f, 0.5f, 0.0f, 1.0f }; //上
	vertexData[2] = { 0.5f, -0.5f, 0.0f, 1.0f }; //右下

	/*マテリアルリソースの作成*/
	materialResource = CreateBufferResource(device, sizeof(Vector4));
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//赤を書き込む
	*materialData = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	/*WVPリソースの作成*/
	wvpResource = CreateBufferResource(device, sizeof(Matrix4x4));
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	*wvpData = Matrix4x4::Identity();

	/*Transformの初期化*/
	transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
}

void Object3D::Update(const Matrix4x4& viewProjectionMatrix) {
	transform.rotation.y += 0.01f;

	//ワールド行列更新
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

	Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
	*wvpData = worldViewProjectionMatrix;
}

void Object3D::Draw(ID3D12GraphicsCommandList* commandList) {
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	//形状を設定。PSOとは別途設定。同じものを設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定(RootParameter配列の0番目)
	commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	//描画(DrawCall)
	commandList->DrawInstanced(3, 1, 0, 0);
}

void Object3D::Finalize() {
	vertexResource->Release();
	materialResource->Release();
	wvpResource->Release();
}
