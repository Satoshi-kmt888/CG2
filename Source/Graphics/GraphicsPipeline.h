#pragma once

#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"

#include <d3d12.h>
#include <dxcapi.h>
#include <wrl/client.h>
#include <vector>
#include <deque>
#include <string>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class RootSignature;

class GraphicsPipeline {
public:
	//--- インスタンス管理 ---

	GraphicsPipeline();
	~GraphicsPipeline() = default;

	//--- 公開関数 ---

	/// <summary>
	/// パイプラインに紐づけるルートシグネチャを設定
	/// </summary>
	/// <param name="rootSignature"></param>
	GraphicsPipeline& SetRootSignature(const RootSignature* rootSignature);

	/// <summary>
	/// 頂点シェーダーを設定
	/// </summary>
	/// <param name="vsBlob"></param>
	GraphicsPipeline& SetVertexShader(IDxcBlob* vsBlob);

	/// <summary>
	/// ピクセルシェーダーを設定
	/// </summary>
	/// <param name="vsBlob"></param>
	GraphicsPipeline& SetPixelShader(IDxcBlob* psBlob);

	/// <summary>
	/// 頂点入力レイアウトを1要素ずつ動的に追加
	/// </summary>
	/// <param name="semanticName">セマンティック名（"POSITION", "TEXCOORD" など）</param>
	/// <param name="format">データの型（DXGI_FORMAT_R32G32B32_FLOAT など）</param>
	/// <param name="semanticIndex">同じセマンティック名がある場合のインデックス（デフォルトは0）</param>
	GraphicsPipeline& AddInputLayout(const std::string& semanticName, DXGI_FORMAT format, UINT semanticIndex = 0);

	/// <summary>
	/// ブレンドステートプリセットを設定
	/// </summary>
	/// <param name="blendState"></param>
	GraphicsPipeline& SetBlendState(const BlendState& blendState);

	/// <summary>
	/// ラスタライザステートプリセットを設定
	/// </summary>
	/// <param name="rasterizerState"></param>
	GraphicsPipeline& SetRasterizerState(const RasterizerState& rasterizerState);

	/// <summary>
	/// デプスステンシルステートプリセットを設定
	/// </summary>
	/// <param name="depthStencilState"></param>
	GraphicsPipeline& SetDepthStencilState(const DepthStencilState& depthStencilState);

	/// <summary>
	/// RTVのフォーマット設定
	/// </summary>
	/// <param name="rtvFormat"></param>
	GraphicsPipeline& SetRenderTargetFormat(DXGI_FORMAT rtvFormat);

	/// <summary>
	/// DSVのフォーマット設定
	/// </summary>
	/// <param name="dsvFormat"></param>
	GraphicsPipeline& SetDepthStencilFormat(DXGI_FORMAT dsvFormat);

	/// <summary>
	/// 生成したパラメータをもとにPSOを構成
	/// </summary>
	/// <param name="device"></param>
	/// <returns></returns>
	[[nodiscard]] bool Build(ID3D12Device* device);

	//--- ゲッター ---

	ID3D12PipelineState* Get() const { return pipelineState_.Get(); }

private:
	//--- 内部関数 ---

	/// <summary>
	/// パイプラインの生成に使用した一時配列と一時バイナリデータをクリア
	/// </summary>
	void Clear();

	//--- 内部変数 ---

	//生成されるPSOオブジェクト
	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	//パイプライン構築用
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_{};

	//頂点レイアウトデータ
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;

	std::deque<std::string> semanticNames_;

	//シェーダーBlob
	ComPtr<IDxcBlob> vsBlob_ = nullptr;
	ComPtr<IDxcBlob> psBlob_ = nullptr;
};
