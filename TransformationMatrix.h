#pragma once
#include "Matrix4x4.h"

/**
 * \struct TransformationMatrix
 * \brief GPUへ送るための座標変換行列データ構造体
 */
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};
