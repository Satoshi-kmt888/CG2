#pragma once

#include <d3d12.h>

struct RasterizerState {
	D3D12_RASTERIZER_DESC desc{};
};

namespace RasterizerStates {
	RasterizerState Default();

	RasterizerState BackCull();

	RasterizerState FrontCull();

	RasterizerState NoCull();

	RasterizerState Wireframe();
}
