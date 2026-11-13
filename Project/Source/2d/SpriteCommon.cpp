#include "SpriteCommon.h"

#include "Debugger/Logger.h"
#include "Pipeline/PipelineObject2D.h"

SpriteCommon* SpriteCommon::GetInstance() {
	static SpriteCommon instance;
	return &instance;
}

bool SpriteCommon::Initialize(ID3D12Device* device) {
	if (!device) {
		LOG_ERROR("引数 device がnullptrです。");
		return false;
	}

	m_pipeline = std::make_unique<PipelineObject2D>();
	if (!m_pipeline->Initialize(device)) {
		LOG_ERROR("Object2D用パイプラインの生成に失敗しました。");
		m_pipeline.reset();
		return false;
	}

	return true;
}

void SpriteCommon::Finalize() {
	m_pipeline.reset();
}

void SpriteCommon::SetPipelineState(ID3D12GraphicsCommandList* command_list) {
	if (!m_pipeline || !command_list) {
		return;
	}

	m_pipeline->Bind(command_list);
}
