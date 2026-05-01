#include "Sphere.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"

#include <cmath>

void Sphere::Initialize(ID3D12DescriptorHeap* descriptorHeap) {
	//SRVを作成するDescriptorHeapの場所を決める
	textureSrvHandleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	//先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	/*頂点リソースの作成*/
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

	/*マテリアルリソースの作成*/
	materialResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Vector4));
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//赤を書き込む
	*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	/*WVPリソースの作成*/
	wvpResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Matrix4x4));
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	*wvpData = Matrix4x4::Identity();

	/*Transformの初期化*/
	transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };


	//緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		//現在の緯度
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;

		//経度の方向に分割
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			//現在の経度
			float lon = lonIndex * kLonEvery;

			//
			float startU = static_cast<float>(lonIndex) / static_cast<float>(kSubdivision);
			float startV = 1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision);
			float nextU = static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision);
			float nextV = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision);

			//左下
			vertexData[start].position.x = std::cos(lat) * std::cos(lon);
			vertexData[start].position.y = std::sin(lat);
			vertexData[start].position.z = std::cos(lat) * std::sin(lon);
			vertexData[start].position.w = 1.0f;
			vertexData[start].texCoord = { startU, startV };

			//左上
			vertexData[start + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			vertexData[start + 1].position.y = std::sin(lat + kLatEvery);
			vertexData[start + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			vertexData[start + 1].position.w = 1.0f;
			vertexData[start + 1].texCoord = { startU, nextV };

			//右下
			vertexData[start + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			vertexData[start + 2].position.y = std::sin(lat);
			vertexData[start + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			vertexData[start + 2].position.w = 1.0f;
			vertexData[start + 2].texCoord = { nextU, startV };

			//右上
			vertexData[start + 3].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			vertexData[start + 3].position.y = std::sin(lat + kLatEvery);
			vertexData[start + 3].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			vertexData[start + 3].position.w = 1.0f;
			vertexData[start + 3].texCoord = { nextU, nextV };

			//右下
			vertexData[start + 4].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			vertexData[start + 4].position.y = std::sin(lat);
			vertexData[start + 4].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			vertexData[start + 4].position.w = 1.0f;
			vertexData[start + 4].texCoord = { nextU, startV };

			//左上
			vertexData[start + 5].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			vertexData[start + 5].position.y = std::sin(lat + kLatEvery);
			vertexData[start + 5].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			vertexData[start + 5].position.w = 1.0f;
			vertexData[start + 5].texCoord = { startU, nextV };
		}
	}
}

void Sphere::Update(const Matrix4x4& viewProjectionMatrix) {
	//回転
	transform.rotation.y += 0.005f;

	//ワールド行列更新
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

	Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
	*wvpData = worldViewProjectionMatrix;
}

void Sphere::Draw() {
	// 2. 描画コマンドの発行
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	// 頂点バッファのセット
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアル・WVP行列・テクスチャをセット (Object3Dと同じ順序)
	commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

	// 全頂点数を描画
	commandList->DrawInstanced(vertexCount, 1, 0, 0);
}
