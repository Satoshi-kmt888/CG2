#include "BaseObject.h"

#include "DirectXCommon.h"
#include "D3D12Util.h"

void BaseObject::CreateVertexBuffer(uint32_t vertexCount){
	//データ書き込み
	vertexResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * vertexCount);
	//リソースの先頭アドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexCount;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
}

void BaseObject::CreateMaterialBuffer(){
	materialResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Vector4));
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//赤を書き込む
	*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void BaseObject::CreateWVPBuffer(){
	wvpResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Matrix4x4));
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	*wvpData = Matrix4x4::Identity();
}

void BaseObject::RecordCommonCommand(ID3D12GraphicsCommandList* commandList){
	//
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	//マテリアル(0)、WVP(1)、テクスチャ(2)をセット
	commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
}
