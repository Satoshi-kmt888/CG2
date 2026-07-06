#include "DirectionalLight.h"

#include "Graphics/D3D12Utility.h"
#include "Graphics/DirectXCommon.h"

DirectionalLight::~DirectionalLight() {
	if (constantBufferResource_ && constantBufferData_) {
		constantBufferResource_->Unmap(0, nullptr);
		constantBufferData_ = nullptr;
	}
}

void DirectionalLight::Initialize() {
	//定数バッファリソースの作成
	constantBufferResource_ = D3D12Utility::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ConstantBufferData));
	//書き込むためのアドレスを取得
	constantBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&constantBufferData_));

	//一度更新を行う
	Update();
}

void DirectionalLight::Update() {
	if (!constantBufferData_) {
		return;
	}

	constantBufferData_->color = color_;
	constantBufferData_->direction = direction_;
	constantBufferData_->intensity = intensity_;
}
