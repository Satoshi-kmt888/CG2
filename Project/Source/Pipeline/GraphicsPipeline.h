#pragma once

#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"

#include <d3d12.h>
#include <deque>
#include <dxcapi.h>
#include <string>
#include <vector>
#include <wrl/client.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class RootSignature;

class GraphicsPipeline {
public:
	//============================================================
	// publice methods
	//============================================================

	GraphicsPipeline();
	~GraphicsPipeline() = default;

	/// <summary>
	/// パイプラインに紐づけるルートシグネチャを設定
	/// </summary>
	/// <param name="rootSignature"></param>
	GraphicsPipeline& SetRootSignature(const RootSignature* root_signature);

	/// <summary>
	/// 頂点シェーダーを設定
	/// </summary>
	/// <param name="vsBlob"></param>
	GraphicsPipeline& SetVertexShader(IDxcBlob* vs_blob);

	/// <summary>
	/// ピクセルシェーダーを設定
	/// </summary>
	/// <param name="vsBlob"></param>
	GraphicsPipeline& SetPixelShader(IDxcBlob* ps_blob);

	/// <summary>
	/// 頂点入力レイアウトを1要素ずつ動的に追加
	/// </summary>
	/// <param name="semanticName">セマンティック名（"POSITION", "TEXCOORD" など）</param>
	/// <param name="format">データの型（DXGI_FORMAT_R32G32B32_FLOAT など）</param>
	/// <param name="semanticIndex">同じセマンティック名がある場合のインデックス（デフォルトは0）</param>
	GraphicsPipeline& AddInputLayout(const std::string& semantic_name, DXGI_FORMAT format, UINT semantic_index = 0);

	/// <summary>
	/// ブレンドステートプリセットを設定
	/// </summary>
	/// <param name="blendState"></param>
	GraphicsPipeline& SetBlendState(const BlendState& blend_state);

	/// <summary>
	/// ラスタライザステートプリセットを設定
	/// </summary>
	/// <param name="rasterizerState"></param>
	GraphicsPipeline& SetRasterizerState(const RasterizerState& rasterizer_state);

	/// <summary>
	/// デプスステンシルステートプリセットを設定
	/// </summary>
	/// <param name="depthStencilState"></param>
	GraphicsPipeline& SetDepthStencilState(const DepthStencilState& depth_stencil_state);

	/// <summary>
	/// RTVのフォーマット設定
	/// </summary>
	/// <param name="rtvFormat"></param>
	GraphicsPipeline& SetRenderTargetFormat(DXGI_FORMAT rtv_format);

	/// <summary>
	/// DSVのフォーマット設定
	/// </summary>
	/// <param name="dsvFormat"></param>
	GraphicsPipeline& SetDepthStencilFormat(DXGI_FORMAT dsv_format);

	/// <summary>
	/// 生成したパラメータをもとにPSOを構成
	/// </summary>
	/// <param name="device"></param>
	/// <returns></returns>
	[[nodiscard]] bool Build(ID3D12Device* device);

	//============================================================
	// accessor
	//============================================================

	ID3D12PipelineState* Get() const { return m_pipeline_state.Get(); }

private:
	//============================================================
	// private methods
	//============================================================

	/// <summary>
	/// パイプラインの生成に使用した一時配列と一時バイナリデータをクリア
	/// </summary>
	void Clear();

	//============================================================
	// private variables
	//============================================================

	//生成されるPSOオブジェクト
	ComPtr<ID3D12PipelineState> m_pipeline_state = nullptr;

	//パイプライン構築用
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc{};

	//頂点レイアウトデータ
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_input_element_descs;

	std::deque<std::string> m_semantic_names;

	//シェーダーBlob
	ComPtr<IDxcBlob> m_vs_blob = nullptr;
	ComPtr<IDxcBlob> m_ps_blob = nullptr;
};
