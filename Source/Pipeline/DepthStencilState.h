#pragma once

#include <d3d12.h>

struct DepthStencilState {
	D3D12_DEPTH_STENCIL_DESC desc{};
};

/// <summary>
/// デプスステンシルステートプリセット
/// </summary>
namespace DepthStencilStates {
	DepthStencilState Default();

	DepthStencilState Disable();

	DepthStencilState ReadOnly();

	DepthStencilState Less();

	DepthStencilState LessEqual();

}
