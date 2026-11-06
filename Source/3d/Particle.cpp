#include "Particle.h"

Particle::Particle() = default;

Particle::~Particle() = default;

void Particle::Initialize() {
	m_model = Model::CreatePlane();
}

void Particle::Update() {

}

void Particle::Draw(const Transform& transform, const Matrix4x4& view_projection_matrix, UINT instance_count) {
	m_model->Draw(transform, view_projection_matrix, instance_count);
}
