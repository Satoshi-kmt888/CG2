#include "Sprite.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"

#include <Windows.h>

Sprite::~Sprite() {
	if (transformationResource_ && transformationData_) {
		transformationResource_->Unmap(0, nullptr);
		transformationData_ = nullptr;
	}
}

void Sprite::Initialize() {
	//座標変換用の定数バッファ作成
	transformationResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));

	//単位行列で初期化
	transformationData_->WVP = Matrix4x4::Identity();
	transformationData_->World = Matrix4x4::Identity();
}

std::unique_ptr<Sprite> Sprite::CreateQuad(const Vector2& size, const std::string& textureFilePath) {
	std::unique_ptr<Sprite> sprite(new Sprite());

	//メッシュセット
	sprite->mesh_ = Mesh::CreateQuad();

	//マテリアルをセット
	sprite->material_->Initialize();
	sprite->material_->SetEnableLighting(false); //ライティングはオフにしておく
	sprite->material_->SetTexture(textureFilePath);

	//モデルを初期化
	sprite->Initialize();
	sprite->scale_.x = size.x;
	sprite->scale_.y = size.y;

	return sprite;
}

void Sprite::Update(const Matrix4x4& viewProjectionMatrix) {
	transformationData_->World = Matrix4x4::MakeAffineMatrix(
		{ scale_.x, scale_.y, 1.0f },
		{ 0.0f, 0.0f, rotation_ },
		{ translation_.x, translation_.y, 0.0f }
	);

	transformationData_->WVP = transformationData_->World * viewProjectionMatrix;

	//マテリアルの更新処理
	material_->Update();
}

void Sprite::Draw() {
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	//メッシュ(形状)をセット
	mesh_->Bind(commandList);

	//マテリアル(素材/テクスチャ)をセット
	material_->Bind(commandList, 0, 2);

	commandList->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());

	if (!mesh_->GetIndices().empty()) {
		commandList->DrawIndexedInstanced(static_cast<UINT>(mesh_->GetIndices().size()), 1, 0, 0, 0);
	} else {
		commandList->DrawInstanced(static_cast<UINT>(mesh_->GetVertices().size()), 1, 0, 0);
	}
}
