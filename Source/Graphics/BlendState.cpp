#include "BlendState.h"

namespace {
	void InitializeRenderTarget(D3D12_RENDER_TARGET_BLEND_DESC& renderTarget) {
		renderTarget.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		renderTarget.LogicOpEnable = FALSE;
		renderTarget.LogicOp = D3D12_LOGIC_OP_NOOP;
	}
}

BlendState BlendStates::None() {
	BlendState state{};

	auto& rt = state.desc.RenderTarget[0];
	InitializeRenderTarget(rt);
	rt.BlendEnable = FALSE;

	return state;
}

BlendState BlendStates::Alpha() {
	BlendState state{};

	auto& rt = state.desc.RenderTarget[0];

	InitializeRenderTarget(rt);

	rt.BlendEnable = TRUE;

	rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	rt.BlendOp = D3D12_BLEND_OP_ADD;

	rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;

	return state;
}

BlendState BlendStates::Add() {
	BlendState state{};

	auto& rt = state.desc.RenderTarget[0];

	InitializeRenderTarget(rt);

	rt.BlendEnable = TRUE;

	rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	rt.DestBlend = D3D12_BLEND_ONE;
	rt.BlendOp = D3D12_BLEND_OP_ADD;

	rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;

	return state;
}

BlendState BlendStates::Subtract() {
	BlendState state{};

	auto& rt = state.desc.RenderTarget[0];

	InitializeRenderTarget(rt);

	rt.BlendEnable = TRUE;

	rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	rt.DestBlend = D3D12_BLEND_ONE;
	rt.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;

	rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;

	return state;
}

BlendState BlendStates::Multiply() {
	BlendState state{};

	auto& rt = state.desc.RenderTarget[0];

	InitializeRenderTarget(rt);

	rt.BlendEnable = TRUE;

	rt.SrcBlend = D3D12_BLEND_ZERO;
	rt.DestBlend = D3D12_BLEND_SRC_COLOR;
	rt.BlendOp = D3D12_BLEND_OP_ADD;

	rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;

	return state;
}
