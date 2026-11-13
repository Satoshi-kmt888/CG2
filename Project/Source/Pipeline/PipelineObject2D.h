#pragma once

#include <d3d12.h>
#include <memory>

class RootSignature;
class GraphicsPipeline;

/// <summary>
/// スプライト用パイプライン
/// </summary>
class PipelineObject2D {
public:
	//============================================================
	// public methods
	//============================================================

	PipelineObject2D();
	~PipelineObject2D();

	/// <summary>
	/// ルートシグネチャ・シェーダーコンパイル・PSOの作成を行う
	/// </summary>
	/// <param name="device"></param>
	/// <returns></returns>
	[[nodiscard]] bool Initialize(ID3D12Device* device);

	/// <summary>
	/// コマンドリストで紐づけを行う
	/// </summary>
	/// <param name="command_list"></param>
	void Bind(ID3D12GraphicsCommandList* command_list);

private:
	//============================================================
	// private variables
	//============================================================

	std::unique_ptr<RootSignature> m_root_signature;
	std::unique_ptr<GraphicsPipeline> m_pipeline;
};

