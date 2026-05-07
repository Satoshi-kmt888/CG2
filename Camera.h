#pragma once

#include <d3d12.h>

#include "Matrix4x4.h"
#include "Vector3.h"

/**
 *  \enum ProjectionType
 *  \brief カメラの投影方式を定義する列挙型
 */
enum class ProjectionType {
	Perspective, //!< 3D
	Orthographic //!< 2D
};

/**
*  \class Camera
 * \brief ビュー行列とプロジェクション行列を管理するクラス
 * \details 透視投影(3D)と正射影(2D)の両方の行列を生成・保持
 */
class Camera {
public://--- コンストラクタ・デストラクタ ---
	Camera();
	~Camera() = default;

public://--- ライフサイクル ---
	/**
	 * \brief 初期化処理
	 * \param[in] width クライアント領域の横幅
	 * \param[in] height クライアント領域の縦幅
	 */
	void Initialize(int width, int height);

	/**
	 * \brief 更新処理
	 * \details
	 */
	void Update();

private://--- 内部関数 ---
	/**
	 * \brief 行列の更新
	 * \details ビュー行列・プロジェクション行列から合成行列を求める
	 */
	void UpdateMatrix();

public://--- ゲッター ---
	const D3D12_VIEWPORT& GetViewport() const { return viewport_; }
	const D3D12_RECT& GetScissorRect() const { return scissorRect_; }

	Vector3& GetRotation() { return rotation_; }
	Vector3& GetTranslation() { return translation_; }

	Matrix4x4 GetViewProjMatrix() const { return viewProjMatrix_; }

public://--- セッター ---
	void SetProjectionType(ProjectionType projectionType) { projectionType_ = projectionType; }

private://--- メンバ変数 ---
	//ワールド変換データ
	Vector3 rotation_;
	Vector3 translation_;

	//透視投影
	float fovY_;
	float aspectRatio_;

	//クリップ範囲
	float nearClip_;
	float farClip_;

	//ビューポート・シザー矩形
	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	//投影の種別
	ProjectionType projectionType_;

	//行列
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewProjMatrix_;
};
