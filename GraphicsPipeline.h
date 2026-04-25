#pragma once
#include <wrl/client.h>

#include <d3d12.h>

class ShaderCompiler;

/**
 * \class GraphicsPipeline
 * \brief パイプラインステートとルートシグネチャを管理するクラス
 * * 役割:
 * - シェーダーを統合し、GPUの描画設定(PSO)を作成・保持する
 * - GPUへのデータの渡し口(RootSignature)を管理する
 */
class GraphicsPipeline {
public: //--- ライフサイクル ---
	/**
	 * \brief 初期化処理
	 * \param[in] shaderCompiler コンパイル済みのシェーダーにアクセスするためのポインタ
	 */
	void Initialize(ShaderCompiler* shaderCompiler);

public: //--- ゲッター ---
	ID3D12RootSignature* GetRootSignature() const { return rootSignature.Get(); }
	ID3D12PipelineState* GetGraphicsPipelineState() const { return graphicsPipelineState.Get(); }

private: //--- メンバ変数 ---
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
};
