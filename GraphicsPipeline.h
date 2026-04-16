#pragma once

#include <d3d12.h>

#include "ShaderCompiler.h"

/// <summary>
/// グラフィックスパイプライン
/// </summary>
class GraphicsPipeline {
public:
	void Initialize(ID3D12Device* device, ShaderCompiler* shaderCompiler);
	void Finalize();

	ID3D12RootSignature* GetRootSignature() const { return rootSignature; }
	ID3D12PipelineState* GetGraphicsPipelineState() const { return graphicsPipelineState; }

private:
	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;
};
