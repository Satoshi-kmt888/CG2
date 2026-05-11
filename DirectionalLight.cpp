#include "DirectionalLight.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"

void DirectionalLight::Initialize(){
	//データ書き込み
	resource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ConstBufferData));
	//書き込むためのアドレスを取得
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&constBufferData_));
}

void DirectionalLight::Update(){
	constBufferData_->direction = direction_;
	constBufferData_->color = color_;
	constBufferData_->intensity = intensity_;
}
