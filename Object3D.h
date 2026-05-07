#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

#include "Matrix4x4.h"
#include "Model.h"
#include "Transform.h"
#include "Vector3.h"
#include "Vector4.h"

/**
 * \struct TransformationMatrix
 * \brief GPUへ送るための座標変換行列データ構造体
 */
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

/**
 * \class Object3D
 * \brief 3Dオブジェクトの描画とデータを管理するクラス
 * \details 特定のモデルを参照し、個別の座標やマテリアル情報などを保持する
 */
class Object3D {
public:
	//==================================================
	// 公開関数
	//==================================================

	Object3D() = default;
	~Object3D();

	//==================================================
	// ライフサイクル
	//==================================================
	/**
	 * \brief 初期化処理
	 * \details マテリアル用、WVP用の定数バッファリソースを生成し、Mapを行う
	 */
	void Initialize();

	/**
	 * \brief 更新処理
	 * \param[in] viewProjectionMatrix カメラのビュープロジェクション行列
	 * \details Transformの情報からWorld行列を計算し、定数バッファにWVP行列を書き込む
	 */
	void Update(const Matrix4x4& viewProjectionMatrix);

	/**
	 * \brief 描画処理
	 * \details 定数バッファをセットし、参照しているModelの描画関数を呼び出す
	 */
	void Draw();

	//==================================================
	// ゲッター
	//==================================================

	Vector3 GetRotation() const { return transform_.rotation; }
	const Vector3& GetTranslation() const { return transform_.translation; }
	Vector3& GetTranslation() { return transform_.translation; }

	const Vector4& GetMaterialColor() const { return model_->GetMaterialData()->color; }
	Vector4& GetMaterialColor() { return model_->GetMaterialData()->color; }

	Vector4 GetDirectionalLightColor() const { return model_->GetDirectionalLight()->color; }
	Vector4& GetDirectionalLightColor() { return model_->GetDirectionalLight()->color; }
	Vector3 GetDirectionalLightDirection() const { return model_->GetDirectionalLight()->direction; }
	Vector3& GetDirectionalLightDirection() { return model_->GetDirectionalLight()->direction; }
	float GetDirectionalLightIntensity() const { return model_->GetDirectionalLight()->intensity; }
	float& GetDirectionalLightIntensity() { return model_->GetDirectionalLight()->intensity; }

	//==================================================
	// セッター
	//==================================================

	void SetRotation(Vector3 rotation) { transform_.rotation = rotation; }
	void SetModel(Model* model) { model_ = model; }
	void SetMaterialEnableLighting(uint32_t enableLighting) const { model_->GetMaterialData()->enableLighting = enableLighting; }

private:
	//==================================================
	// メンバ変数
	//==================================================

	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;

	//トランスフォーム
	Transform transform_{};

	//書き込み用のデータアドレス
	MaterialData* materialData_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	//描画に使用する形状データポインタ
	Model* model_ = nullptr;
};
