#pragma once

#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

struct Particle {
	Transform transform;
	Vector3 velocity;
	Vector4 color;
	float life_time;
	float current_time;
};
