#include "Sprite.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"
#include "Mesh.h"
#include "Material.h"

void Sprite::Initialize() {
    //座標変換用の定数バッファ作成
    transformationResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
    transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));

    //単位行列で初期化
    transformationData_->WVP = Matrix4x4::Identity();
    transformationData_->World = Matrix4x4::Identity();

    //Materialの生成・初期化
    material_ = std::make_unique<Material>();
    material_->Initialize();
    material_->SetEnableLighting(false); // スプライトは基本ライティングOFF
}

void Sprite::Update(const Matrix4x4& viewProjectionMatrix) {
    transformationData_->World = Matrix4x4::MakeAffineMatrix(
        {size_.x, size_.y, 1.0f},
        { 0.0f, 0.0f, rotation_ },
        {position_.x, position_.y, 0.0f}
    );

    transformationData_->WVP = transformationData_->World * viewProjectionMatrix;
}

void Sprite::Draw() {
    auto commandList = DirectXCommon::GetInstance()->GetCommandList();

    //メッシュ(形状)をセット
    mesh_->Bind(commandList);

    //マテリアル(素材/テクスチャ)をセット
    material_->Bind(commandList, 0, 2);

    commandList->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());

    if (mesh_->GetIndexCount() > 0) {
        commandList->DrawIndexedInstanced(static_cast<UINT>(mesh_->GetIndexCount()), 1, 0, 0, 0);
    } else {
        commandList->DrawInstanced(static_cast<UINT>(mesh_->GetVertexCount()), 1, 0, 0);
    }
}
