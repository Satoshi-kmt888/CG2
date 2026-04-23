#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include "ShaderCompiler.h"

/// <summary>
/// グラフィックスパイプライン
/// </summary>
class GraphicsPipeline {
public:
	void Initialize(ID3D12Device* device, ShaderCompiler* shaderCompiler);
	void Finalize();

	ID3D12RootSignature* GetRootSignature() const { return rootSignature.Get(); }
	ID3D12PipelineState* GetGraphicsPipelineState() const { return graphicsPipelineState.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
};
