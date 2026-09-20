#include "Scene/Sprite.h"

#include "Debugger/Logger.h"
#include "Graphics/D3D12Utility.h"
#include "Graphics/GraphicsSystem.h"

Sprite::~Sprite() {
	if (transformationBuffer_ && transformationData_) {
		transformationBuffer_->Unmap(0, nullptr);
		transformationData_ = nullptr;
	}
}

std::unique_ptr<Sprite> Sprite::Create() {
	//デバイスを取得
	ID3D12Device* device = GraphicsSystem::GetInstance()->GetDevice();

	//各インスタンスを生成
	auto sprite = std::make_unique<Sprite>();
	sprite->material_ = std::make_unique<Material>();
	sprite->mesh_ = std::make_unique<Mesh>();

	//頂点データを配列に格納
	Mesh::VertexData vertices[] = {
		{ {-0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }, // 左上
		{ {0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }, // 右上
		{ {-0.5f, 0.5f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }, // 左下
		{ {0.5f, 0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }, // 右下
	};

	//メッシュ側に転送
	for (const auto& v : vertices) {
		Mesh::VertexData vertex{};
		vertex.position = { v.position.x, v.position.y, v.position.z, 1.0f };
		vertex.texCoord.x = v.texCoord.x;
		vertex.texCoord.y = v.texCoord.y;
		vertex.normal = v.normal;

		sprite->mesh_->AddVertex(vertex);
	}

	//インデックスを追加
	sprite->mesh_->AddIndex(0);
	sprite->mesh_->AddIndex(1);
	sprite->mesh_->AddIndex(2);
	sprite->mesh_->AddIndex(1);
	sprite->mesh_->AddIndex(3);
	sprite->mesh_->AddIndex(2);

	//メッシュバッファを作る
	sprite->mesh_->CreateBuffer(device);

	//マテリアルにテクスチャをセットしてバッファを作る
	sprite->material_->SetTexture("Resources/uvChecker.png");
	sprite->material_->CreateBuffer(device);
	sprite->material_->SetLightType(0); //ライティングはオフ

	//スプライト本体を作る
	sprite->CreateBuffer(device);

	return sprite;
}

void Sprite::Draw(Transform& transform, const Matrix4x4& viewProjectionMatrix) {
	auto commandList = GraphicsSystem::GetInstance()->GetCommandList();

	if (!transformationBuffer_) {
		LOG_ERROR("Model::Draw メンバ変数transformationBuffer_がnullptrです。Model::CreateBufferでtransformationBuffer_を作成してください。");
		return;
	}

	//ワールド変換データを更新
	transform.scale.z = 1.0f;
	transform.rotation = { 0.0f, 0.0f, transform.rotation.z }; //Rollだけにする
	transform.translation.z = 0.0f;
	transformationData_->world = Transform::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);
	transformationData_->wvp = transformationData_->world * viewProjectionMatrix;
	commandList->SetGraphicsRootConstantBufferView(1, transformationBuffer_->GetGPUVirtualAddress());

	//マテリアルを更新
	material_->Update();


	//コマンド実行
	material_->SetGraphicsCommand(commandList, 0, 2);
	mesh_->Draw(commandList);
}

void Sprite::CreateBuffer(ID3D12Device* device) {
	transformationBuffer_ = D3D12Utility::CreateBufferResource(device, sizeof(TransformationMatrix));
	if (!transformationBuffer_) {
		LOG_ERROR("Model::CreateBuffer メンバ変数transformationBuffer_がnullptrです。Model::CreateBufferでtransformationBuffer_を作成してください。");
		return;
	}

	void* transformationPtr = nullptr;
	HRESULT hr = transformationBuffer_->Map(0, nullptr, &transformationPtr);
	if (FAILED(hr)) {
		LOG_ERROR("Model::CreateBuffer メンバ変数transformationBufferをマッピングすることができませんでした。");
		return;
	}

	transformationData_ = static_cast<TransformationMatrix*>(transformationPtr);
	if (transformationData_) {
		transformationData_->wvp = Matrix4x4::Identity();
		transformationData_->world = Matrix4x4::Identity();
	}
}
