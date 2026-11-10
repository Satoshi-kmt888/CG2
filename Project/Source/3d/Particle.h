#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/WorldTransform.h"

struct Particle {
	WorldTransform transform;
	Vector3 velocity;
	Vector4 color;
	float life_time;
	float current_time;
};
