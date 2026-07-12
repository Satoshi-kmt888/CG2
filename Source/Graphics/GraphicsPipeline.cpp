#include "GraphicsPipeline.h"

#include "RootSignature.h"
#include "Debugger/Logger.h"

GraphicsPipeline::GraphicsPipeline() {
	//各ステートのデフォルト設定
	desc_.BlendState = BlendStates::None().desc;
	desc_.RasterizerState = RasterizerStates::Default().desc;
	desc_.DepthStencilState = DepthStencilStates::Default().desc;

	//レンダーターゲット・サンプリング関連の標準値
	desc_.NumRenderTargets = 1;
	desc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc_.SampleDesc.Count = 1;                           // マルチサンプルなし
	desc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 3. 深度トポロジ関連の標準値を設定
	desc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 三角形描画
	desc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
}

GraphicsPipeline& GraphicsPipeline::SetRootSignature(const RootSignature* rootSignature) {
	if (!rootSignature) {
		LOG_ERROR("引数 rootSignature がnullptrです。");
		return *this;
	}

	desc_.pRootSignature = rootSignature->Get();
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetVertexShader(IDxcBlob* vsBlob) {
	if (!vsBlob) {
		LOG_ERROR("引数 vsBlob がnullptrです。");
		return *this;
	}

	vsBlob_ = vsBlob;
	desc_.VS = { vsBlob_->GetBufferPointer(), vsBlob_->GetBufferSize() };
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetPixelShader(IDxcBlob* psBlob) {
	if (!psBlob) {
		LOG_ERROR("引数 vsBlob がnullptrです。");
		return *this;
	}

	vsBlob_ = psBlob;
	desc_.PS = { psBlob_->GetBufferPointer(), psBlob_->GetBufferSize() };
	return *this;
}

GraphicsPipeline& GraphicsPipeline::AddInputLayout(const std::string& semanticName, DXGI_FORMAT format, UINT semanticIndex) {
	semanticNames_.push_back(semanticName);

	D3D12_INPUT_ELEMENT_DESC element{};
	element.SemanticName = semanticNames_.back().c_str();
	element.SemanticIndex = semanticIndex;
	element.Format = format;
	element.InputSlot = 0;
	element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

	inputElementDescs_.push_back(element);
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetBlendState(const BlendState& blendState) {
	desc_.BlendState = blendState.desc;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetRasterizerState(const RasterizerState& rasterizerState) {
	desc_.RasterizerState = rasterizerState.desc;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetDepthStencilState(const DepthStencilState& depthStencilState) {
	desc_.DepthStencilState = depthStencilState.desc;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetRenderTargetFormat(DXGI_FORMAT rtvFormat) {
	desc_.RTVFormats[0] = rtvFormat;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetDepthStencilFormat(DXGI_FORMAT dsvFormat) {
	desc_.DSVFormat = dsvFormat;
	return *this;
}

bool GraphicsPipeline::Build(ID3D12Device* device) {
	if (!device) {
		LOG_ERROR("引数 device がnullptrです。");
		return false;
	}

	if (!desc_.pRootSignature) {
		LOG_ERROR("pRootSignature がnullptrです。SetRootSignatureでrootSignatureを設定しください。");
		return false;
	}
	if (!desc_.VS.pShaderBytecode) {
		LOG_ERROR("VertexShaderが設定されていません。");
		return false;
	}
	if (inputElementDescs_.empty()) {
		LOG_ERROR("InputElementの中身が空です。");
		return false;
	}

	desc_.InputLayout.pInputElementDescs = inputElementDescs_.data();
	desc_.InputLayout.NumElements = static_cast<UINT>(inputElementDescs_.size());

	HRESULT hr = device->CreateGraphicsPipelineState(&desc_, IID_PPV_ARGS(&pipelineState_));
	if (FAILED(hr)) {
		//LOG_ERROR("PSOの生成に失敗しました。 HRESULT: ", hr);
		return false;
	}

	//生成に使用した配列とバイナリデータをクリア
	Clear();

	return true;
}

void GraphicsPipeline::Clear() {
	inputElementDescs_.clear();
	inputElementDescs_.shrink_to_fit();
	semanticNames_.clear();
	semanticNames_.shrink_to_fit();

	vsBlob_ = nullptr;
	psBlob_ = nullptr;
}
