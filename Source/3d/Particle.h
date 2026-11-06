#pragma once

#include "3d/Model.h"

#include <memory>

/// <summary>
/// 
/// </summary>
class Particle {
public:
	//=========================================================
	// public methods
	//=========================================================

	Particle();
	~Particle();

	void Initialize();
	void Update();
	void Draw(const Transform& transform, const Matrix4x4& view_projection_matrix, UINT instance_count);

private:
	//=========================================================
	// private methods
	//=========================================================

	//=========================================================
	// private variables
	//=========================================================

	std::unique_ptr<Model> m_model;
};

