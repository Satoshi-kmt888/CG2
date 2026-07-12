#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <d3d12.h>
#include <wrl/client.h>

/**
 * \class DirectionalLight
 * \brief 平行光源を管理するクラス
 */
class DirectionalLight {
public:
	//--- 内部データ構造体 ---

	/**
	* \struct DirectionalLightData
	* \brief GPU側の定数バッファへ転送するためのライトデータ構造体
	*/
	struct ConstantBufferData {
		Vector4 color;     //<! ライトの色
		Vector3 direction; //<! ライトの向き
		float intensity;   //<! 輝度
	};

	//--- コンストラクタ・デストラクタ ---

	DirectionalLight() = default;
	~DirectionalLight();

	//--- 公開関数 ---

	/**
	 * \brief ライトの初期化
	 * \details 定数バッファの生成とMap、初期値の設定
	 */
	void Initialize();

	/**
	 * \brief データ更新
	 * \details CPU側の設定値をGPU側の定数バッファへ書き込む
	 */
	void Update();

	//--- ゲッター ---

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constantBufferResource_->GetGPUVirtualAddress();
	}

	const Vector3& GetDirection() const { return direction_; }
	const Vector4& GetColor() const { return color_; }
	const float& GetIntensity() const { return intensity_; }

	//--- セッター ---

	void SetDirection(const Vector3& direction) { direction_ = direction; }
	void SetColor(const Vector4& color) { color_ = color; }
	void SetIntensity(const float intensity) { intensity_ = intensity; }

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
