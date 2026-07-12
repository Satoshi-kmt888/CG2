#include "DepthStencilState.h"

namespace {
	void InitializeDepthStencil(D3D12_DEPTH_STENCIL_DESC& desc) {
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		desc.StencilEnable = FALSE;

		desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

		desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		desc.BackFace = desc.FrontFace;
	}
}

DepthStencilState DepthStencilStates::Default() {
	DepthStencilState state{};

	InitializeDepthStencil(state.desc);

	return state;
}

DepthStencilState DepthStencilStates::Disable() {
	DepthStencilState state{};

	InitializeDepthStencil(state.desc);

	state.desc.DepthEnable = FALSE;

	return state;
}

DepthStencilState DepthStencilStates::ReadOnly() {
	DepthStencilState state{};

	InitializeDepthStencil(state.desc);

	state.desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	return state;
}

DepthStencilState DepthStencilStates::Less() {
	DepthStencilState state{};

	InitializeDepthStencil(state.desc);

	state.desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	return state;
}

DepthStencilState DepthStencilStates::LessEqual() {
	DepthStencilState state{};

	InitializeDepthStencil(state.desc);

	state.desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	return state;
}
