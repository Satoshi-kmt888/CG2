#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <d3d12.h>
#include <wrl/client.h>

/// <summary>
/// 平行光源を管理するクラス
/// </summary>
class DirectionalLight {
public:
	//--- 内部データ構造体 ---

	/// <summary>
	/// GPU側の定数バッファへ転送するためのライトデータ
	/// </summary>
	struct ConstantBufferData {
		Vector4 color;     //<! ライトの色
		Vector3 direction; //<! ライトの向き
		float intensity;   //<! 輝度
	};

	//--- インスタンス管理 ---

	DirectionalLight() = default;
	~DirectionalLight();

	//--- 公開関数 ---

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	//--- ゲッター ---

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constantBufferResource_->GetGPUVirtualAddress();
	}

private:
	//--- メンバ変数 ---

	//CPU側の設定値
	Vector3 direction_ = { 0.0f, -1.0f, 0.0f };
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	float intensity_ = 1.0f;

	//GPUリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferResource_ = nullptr;
	ConstantBufferData* constantBufferData_ = nullptr;
};
