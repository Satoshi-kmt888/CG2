#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include <dxcapi.h>

class ShaderCompiler;

/**
 * \class GraphicsPipeline
 * \brief パイプラインステートとルートシグネチャを管理するクラス
 * * 役割:
 * - シェーダーを統合し、GPUの描画設定(PSO)を作成・保持する
 * - GPUへのデータの渡し口(RootSignature)を管理する
 */
class GraphicsPipeline {
public:
	//==================================================
	// ライフサイクル
	//==================================================

	/**
	 * \brief 初期化処理
	 * \param[in] shaderCompiler コンパイル済みのシェーダーにアクセスするためのポインタ
	 */
	void Initialize(ShaderCompiler* shaderCompiler);

	//==================================================
	// ゲッター
	//==================================================

	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
	ID3D12PipelineState* GetGraphicsPipelineState() const { return graphicsPipelineState_.Get(); }

private:
	//==================================================
	// 内部関数
	//==================================================

	/** \brief ルートシグネチャの作成*/
	void CreateRootSignature();

	/** \brief インプットレイアウトの設定 */
	void CreateInputLayout();

	/** \brief ブレンドステートの設定 */
	void CreateBlendState();

	/** \brief ラスタライザステートの設定 */
	void CreateRasterizerState();

	/** \brief 深度ステンシルステートの設定 */
	void CreateDepthStencilState();

	/**
	 * \brief パイプラインステートオブジェクト(PSO)の生成
	 * \details 各ステートとコンパイル済みシェーダを統合し、最終的な描画ルールを構築する
	 * \param[in] shaderCompiler シェーダのコンパイルに使用するコンパイラ
	 */
	void CreatePipelineState(ShaderCompiler* shaderCompiler);

	//==================================================
	// メンバ変数
	//==================================================

	//パイプラインオブジェクト
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	//設定データ
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[2]{}; //!< インプットレイアウトの要素実体
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{}; //!< インプットレイアウトの設定
	D3D12_BLEND_DESC blendDesc_{}; //!< ブレンドステートの設定
	D3D12_RASTERIZER_DESC rasterizerDesc_{}; //!< ラスタライザステートの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{}; //!< 深度ステンシルステートの設定
};
