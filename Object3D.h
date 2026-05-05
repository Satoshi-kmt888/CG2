#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "Matrix4x4.h"
#include "Model.h"
#include "Transform.h"
#include "Vector4.h"

/**
 * \class Object3D
 * \brief 3Dオブジェクトの描画とデータを管理するクラス
 * \details 特定のモデルを参照し、個別の座標やマテリアル情報などを保持する
 */
class Object3D {
public:
	//--- コンストラクタ ---
	Object3D();
	~Object3D();

	//--- ライフサイクル ---
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

	//--- ゲッター ---
	Vector3 GetRotation() const { return transform_.rotation; }
	const Vector3& GetTranslation() const { return transform_.translation; }
	Vector3& GetTranslation() { return transform_.translation; }

	//--- セッター ---
	void SetRotation(Vector3 rotation) { transform_.rotation = rotation; }
	void SetModel(Model* model) { model_ = model; }

private:
	//--- メンバ変数 ---
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_ = nullptr;

	//描画に使用する形状データポインタ
	Model* model_ = nullptr;

	//書き込み用のデータアドレス
	Vector4* materialData_ = nullptr;
	Matrix4x4* wvpData_ = nullptr;

	//トランスフォーム
	Transform transform_;
};
