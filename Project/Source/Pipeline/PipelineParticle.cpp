#include "PipelineParticle.h"

#include "Debugger/Logger.h"
#include "GraphicsPipeline.h"
#include "RootSignature.h"
#include "ShaderCompiler.h"

PipelineParticle::PipelineParticle() = default;

PipelineParticle::~PipelineParticle() = default;

bool PipelineParticle::Initialize(ID3D12Device* device) {
	//--- ルートシグネチャを作成 ---

	D3D12_DESCRIPTOR_RANGE range{};
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 1;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	m_root_signature = std::make_unique<RootSignature>();
	m_root_signature->AddCBV(0, D3D12_SHADER_VISIBILITY_PIXEL);                                 //マテリアル
	m_root_signature->AddDescriptorTable({ range }, D3D12_SHADER_VISIBILITY_PIXEL);             //テクスチャ
	m_root_signature->AddDescriptorTable({ range }, D3D12_SHADER_VISIBILITY_VERTEX);            //インスタンシング
	m_root_signature->AddCBV(1, D3D12_SHADER_VISIBILITY_PIXEL);                                 //ライト
	m_root_signature->AddStaticSampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_SHADER_VISIBILITY_PIXEL
	);

	if (!m_root_signature->Build(device)) {
		LOG_ERROR("PipelineParticle: ルートシグネチャの作成に失敗しました。");
		return false;
	}

	//--- シェーダーコンパイル ---

	ShaderCompiler compiler{};
	if (!compiler.Initialize()) {
		return false;
	}

	auto vs_blob = compiler.Compile(L"Resource/Shader/Particle.VS.hlsl", L"vs_6_0");
	auto ps_blob = compiler.Compile(L"Resource/Shader/Particle.PS.hlsl", L"ps_6_0");
	if (!vs_blob || !ps_blob) {
		LOG_ERROR("PipelineParticle: シェーダーのコンパイルに失敗しました。");
		return false;
	}

	//--- PSOを作成 ---

	m_pipeline = std::make_unique<GraphicsPipeline>();
	m_pipeline->SetRootSignature(m_root_signature.get())
		.SetVertexShader(vs_blob.Get())
		.SetPixelShader(ps_blob.Get())
		.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT, 0)
		.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 0)
		.AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 0)
		.SetBlendState(BlendStates::Add())
		.SetRasterizerState(RasterizerStates::BackCull())
		.SetDepthStencilState(DepthStencilStates::ReadOnly());

	if (!m_pipeline->Build(device)) {
		LOG_ERROR("PipelineParticle: PSOの生成に失敗しました。");
		return false;
	}

	LOG_INFO("PipelineParticleの初期化に成功しました。");

	return true;
}

void PipelineParticle::Bind(ID3D12GraphicsCommandList* command_list) {
	command_list->SetGraphicsRootSignature(m_root_signature->Get());
	command_list->SetPipelineState(m_pipeline->Get());
	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
