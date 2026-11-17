#pragma once

#include "Math/WorldTransform.h"

#include <cstdint>

struct Emitter {
	WorldTransform world_transform;
	uint32_t count;
	float frequency;
	float frequency_time;
};
