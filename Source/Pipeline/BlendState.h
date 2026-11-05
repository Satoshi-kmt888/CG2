#pragma once

#include <d3d12.h>

struct BlendState {
	D3D12_BLEND_DESC desc{};
};

namespace BlendStates {
	BlendState None();

	BlendState Alpha();

	BlendState Add();

	BlendState Subtract();

	BlendState Multiply();
}
