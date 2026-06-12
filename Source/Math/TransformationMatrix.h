#pragma once

#include "Matrix4x4.h"

/// @brief GPU転送用の行列データ
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};
