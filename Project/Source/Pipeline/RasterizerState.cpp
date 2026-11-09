#include "RasterizerState.h"

namespace {
	void InitializeRasterizer(D3D12_RASTERIZER_DESC& desc) {
		desc.FillMode = D3D12_FILL_MODE_SOLID;
		desc.CullMode = D3D12_CULL_MODE_BACK;

		desc.FrontCounterClockwise = FALSE;

		desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;

		desc.DepthClipEnable = TRUE;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;

		desc.ForcedSampleCount = 0;

		desc.ConservativeRaster =
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}
}

RasterizerState RasterizerStates::Default() {
	RasterizerState state{};

	InitializeRasterizer(state.desc);

	return state;
}

RasterizerState RasterizerStates::BackCull() {
	RasterizerState state{};

	InitializeRasterizer(state.desc);

	state.desc.CullMode = D3D12_CULL_MODE_BACK;

	return state;
}

RasterizerState RasterizerStates::FrontCull() {
	RasterizerState state{};

	InitializeRasterizer(state.desc);

	state.desc.CullMode = D3D12_CULL_MODE_FRONT;

	return state;
}

RasterizerState RasterizerStates::NoCull() {
	RasterizerState state{};

	InitializeRasterizer(state.desc);

	state.desc.CullMode = D3D12_CULL_MODE_NONE;

	return state;
}

RasterizerState RasterizerStates::Wireframe() {
	RasterizerState state{};

	InitializeRasterizer(state.desc);

	state.desc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	return state;
}
