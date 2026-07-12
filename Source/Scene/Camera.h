#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

#include <numbers>

/**
 * \enum ProjectionType
 * \brief 投影法式
 */
enum class ProjectionType {
	Perspective,
	Orthographic
};

/**
 * \class Camera
 * \brief 描画に必要な行列を管理するクラス
 */
class Camera {
public:
	//--- コンストラクタ・デストラクタ ---

	Camera(float width, float height);
	~Camera() = default;

	//--- 公開関数 ---

	/** \brief 初期化処理 */
	void Initialize(float width, float height);

	/** \brief 更新処理 */
	void Update();

	//--- ゲッター ---

	const Vector3& GetRotation() const { return rotation_; }
	const Vector3& GetTranslation() const { return translation_; }

	float GetFovY() const { return fovY_; }
	float GetAspectRatio() const { return width_ / height_; }
	float GetWidth() const { return width_; }
	float GetHeight() const { return height_; }
	float GetNearClip() const { return nearClip_; }
	float GetFarClip() const { return farClip_; }

	bool GetIsDirty() const { return isDirty_; }

	ProjectionType GetProjectionType() const { return projectionType_; }

	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjMatrix() const { return viewProjectionMatrix_; }

	//--- セッター ---

	void SetRotation(const Vector3& rotation);
	void SetTranslation(const Vector3& translation);

	void SetFovY(float fovY);
	void SetWidth(float width);
	void SetHeight(float height);

	void SetProjectionType(ProjectionType projectionType);

private:
	//--- 内部関数 ---

	/** \brief 行列の更新 */
	void UpdateMatrix();

	//透視投影行列
	Matrix4x4 MakePerspectiveMatrix() const noexcept;

	//正射影行列
	Matrix4x4 MakeOrthographicMatrix() const noexcept;

	//--- 内部変数 ---

	//ワールド変換
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	Vector3 translation_ = { 0.0f, 0.0f, -10.0f };

	//投影パラメータ
	float fovY_ = 45.0f * std::numbers::pi_v<float> / 180.0f;
	float width_ = 1280.0f;
	float height_ = 720.0f;
	float nearClip_ = 0.1f;
	float farClip_ = 1000.0f;

	ProjectionType projectionType_ = ProjectionType::Perspective;
	bool isDirty_ = true;

	//行列
	Matrix4x4 viewMatrix_ = Matrix4x4::Identity();
	Matrix4x4 projectionMatrix_ = Matrix4x4::Identity();
	Matrix4x4 viewProjectionMatrix_ = Matrix4x4::Identity();
};
