#include "GraphicsPipeline.h"

#include "Debugger/Logger.h"
#include "RootSignature.h"

GraphicsPipeline::GraphicsPipeline() {
	//各ステートのデフォルト設定
	m_desc.BlendState = BlendStates::None().desc;
	m_desc.RasterizerState = RasterizerStates::Default().desc;
	m_desc.DepthStencilState = DepthStencilStates::Default().desc;

	//レンダーターゲット・サンプリング関連の標準値
	m_desc.NumRenderTargets = 1;
	m_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	m_desc.SampleDesc.Count = 1;                           // マルチサンプルなし
	m_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 3. 深度トポロジ関連の標準値を設定
	m_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 三角形描画
	m_desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
}

GraphicsPipeline& GraphicsPipeline::SetRootSignature(const RootSignature* root_signature) {
	if (!root_signature) {
		LOG_ERROR("引数 rootSignature がnullptrです。");
		return *this;
	}

	m_desc.pRootSignature = root_signature->Get();
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetVertexShader(IDxcBlob* vs_blob) {
	if (!vs_blob) {
		LOG_ERROR("引数 vsBlob がnullptrです。");
		return *this;
	}

	m_vs_blob = vs_blob;
	m_desc.VS = { m_vs_blob->GetBufferPointer(), m_vs_blob->GetBufferSize() };
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetPixelShader(IDxcBlob* ps_blob) {
	if (!ps_blob) {
		LOG_ERROR("引数 psBlob がnullptrです。");
		return *this;
	}

	m_ps_blob = ps_blob;
	m_desc.PS = { m_ps_blob->GetBufferPointer(), m_ps_blob->GetBufferSize() };
	return *this;
}

GraphicsPipeline& GraphicsPipeline::AddInputLayout(const std::string& semantic_name, DXGI_FORMAT format, UINT semantic_index) {
	m_semantic_names.push_back(semantic_name);

	D3D12_INPUT_ELEMENT_DESC element{};
	element.SemanticName = m_semantic_names.back().c_str();
	element.SemanticIndex = semantic_index;
	element.Format = format;
	element.InputSlot = 0;
	element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

	m_input_element_descs.push_back(element);
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetBlendState(const BlendState& blend_state) {
	m_desc.BlendState = blend_state.desc;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetRasterizerState(const RasterizerState& rasterizer_state) {
	m_desc.RasterizerState = rasterizer_state.desc;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetDepthStencilState(const DepthStencilState& depth_stencil_state) {
	m_desc.DepthStencilState = depth_stencil_state.desc;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetRenderTargetFormat(DXGI_FORMAT rtv_format) {
	m_desc.RTVFormats[0] = rtv_format;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::SetDepthStencilFormat(DXGI_FORMAT dsv_format) {
	m_desc.DSVFormat = dsv_format;
	return *this;
}

bool GraphicsPipeline::Build(ID3D12Device* device) {
	if (!device) {
		LOG_ERROR("引数 device がnullptrです。");
		return false;
	}

	if (!m_desc.pRootSignature) {
		LOG_ERROR("pRootSignature がnullptrです。SetRootSignatureでrootSignatureを設定しください。");
		return false;
	}
	if (!m_desc.VS.pShaderBytecode) {
		LOG_ERROR("VertexShaderが設定されていません。");
		return false;
	}
	if (m_input_element_descs.empty()) {
		LOG_ERROR("InputElementの中身が空です。");
		return false;
	}

	m_desc.InputLayout.pInputElementDescs = m_input_element_descs.data();
	m_desc.InputLayout.NumElements = static_cast<UINT>(m_input_element_descs.size());

	HRESULT hr = device->CreateGraphicsPipelineState(&m_desc, IID_PPV_ARGS(&m_pipeline_state));
	if (FAILED(hr)) {
		//LOG_ERROR("PSOの生成に失敗しました。 HRESULT: ", hr);
		return false;
	}

	//生成に使用した配列とバイナリデータをクリア
	Clear();

	return true;
}

void GraphicsPipeline::Clear() {
	m_input_element_descs.clear();
	m_input_element_descs.shrink_to_fit();
	m_semantic_names.clear();
	m_semantic_names.shrink_to_fit();

	m_vs_blob = nullptr;
	m_ps_blob = nullptr;
}
