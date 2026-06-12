#pragma once

#include "Vector3.h"

/// @brief ワールド変換データ
struct WorldTransform {
	Vector3 scale{ 1.0f, 1.0f, 1.0f };
	Vector3 rotation{ 0.0f, 0.0f, 0.0f };
	Vector3 translation{ 0.0f, 0.0f, 0.0f };
};
