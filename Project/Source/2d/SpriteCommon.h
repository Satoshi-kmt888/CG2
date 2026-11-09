#pragma once

#include <d3d12.h>
#include <memory>

class PipelineObject3D;

/// <summary>
/// 
/// </summary>
class SpriteCommon {
public:
	//=========================================================
	// public methods
	//=========================================================

	static SpriteCommon* GetInstance();

	//コピー・ムーブ禁止
	SpriteCommon(const SpriteCommon&) = delete;
	SpriteCommon& operator=(const SpriteCommon&) = delete;
	SpriteCommon(SpriteCommon&&) = delete;
	SpriteCommon& operator=(SpriteCommon&&) = delete;

	[[nodiscard]] bool Initialize(ID3D12Device* device);

	void Finalize();

	void SetPipelineState(ID3D12GraphicsCommandList* command_list);

private:
	//=========================================================
	// private methods
	//=========================================================

	SpriteCommon() = default;
	~SpriteCommon() = default;

	//=========================================================
	// private methods
	//=========================================================

	std::unique_ptr<PipelineObject3D> m_pipeline;
};
