#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

#include <numbers>

/// <summary>
/// 投影法式
/// </summary>
enum class ProjectionType {
	Perspective,
	Orthographic
};

/// <summary>
/// 描画に必要な行列を管理するクラス
/// </summary>
class Camera {
public:
	//=========================================================
	// public methods
	//=========================================================

	Camera(float width, float height);
	~Camera() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void Initialize(float width, float height);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	//=========================================================
	// accessor
	//=========================================================

	const Vector3& GetRotation() const { return m_rotation; }
	const Vector3& GetTranslation() const { return m_translation; }

	float GetFovY() const { return m_fovY; }
	float GetAspectRatio() const { return m_width / m_height; }
	float GetWidth() const { return m_width; }
	float GetHeight() const { return m_height; }
	float GetNearClip() const { return m_near_clip; }
	float GetFarClip() const { return m_far_clip; }

	bool GetIsDirty() const { return isDirty_; }

	ProjectionType GetProjectionType() const { return projectionType_; }

	const Matrix4x4& GetViewMatrix() const { return m_mat_view; }
	const Matrix4x4& GetProjectionMatrix() const { return m_mat_projection; }
	const Matrix4x4& GetViewProjMatrix() const { return m_mat_view_projection; }

	//---------------------------------------------------------

	void SetRotation(const Vector3& rotation);
	void SetTranslation(const Vector3& translation);

	void SetFovY(float fovY);
	void SetWidth(float width);
	void SetHeight(float height);

	void SetProjectionType(ProjectionType projectionType);

private:
	//=========================================================
	// private methods
	//=========================================================

	/// <summary>
	/// 行列更新
	/// </summary>
	void UpdateMatrix();

	//=========================================================
	// private variables
	//=========================================================

	//ワールド変換
	Vector3 m_translation = { 0.0f, 0.0f, -10.0f };
	Vector3 m_rotation = { 0.0f, 0.0f, 0.0f };

	//投影パラメータ
	float m_fovY = 45.0f * std::numbers::pi_v<float> / 180.0f;
	float m_width = 1280.0f;
	float m_height = 720.0f;
	float m_near_clip = 0.1f;
	float m_far_clip = 1000.0f;

	ProjectionType projectionType_ = ProjectionType::Perspective;
	bool isDirty_ = true;

	//行列
	Matrix4x4 m_mat_world = Matrix4x4::Identity();
	Matrix4x4 m_mat_view = Matrix4x4::Identity();
	Matrix4x4 m_mat_projection = Matrix4x4::Identity();
	Matrix4x4 m_mat_view_projection = Matrix4x4::Identity();
};
