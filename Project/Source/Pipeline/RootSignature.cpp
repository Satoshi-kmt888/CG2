#include "RootSignature.h"

#include "Debugger/Logger.h"

void RootSignature::AddCBV(UINT shader_register, D3D12_SHADER_VISIBILITY visibility) {
	D3D12_ROOT_PARAMETER parameter{};
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	parameter.Descriptor.ShaderRegister = shader_register;
	parameter.Descriptor.RegisterSpace = 0;
	parameter.ShaderVisibility = visibility;

	m_root_parameters.emplace_back(parameter);
}

void RootSignature::AddDescriptorTable(const std::vector<D3D12_DESCRIPTOR_RANGE>& ranges, D3D12_SHADER_VISIBILITY visibility) {
	if (ranges.empty()) {
		return;
	}

	size_t startIndex = m_descriptor_ranges.size();
	for (const auto& range : ranges) {
		m_descriptor_ranges.push_back(range);
	}

	D3D12_ROOT_PARAMETER parameter{};
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	   //DescriptorTableを使う
	parameter.ShaderVisibility = visibility;
	parameter.DescriptorTable.pDescriptorRanges = &m_descriptor_ranges[startIndex];
	parameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(ranges.size());

	m_root_parameters.emplace_back(parameter);
}

void RootSignature::AddStaticSampler(UINT shader_register, D3D12_FILTER filter,
	D3D12_TEXTURE_ADDRESS_MODE address_mode, D3D12_SHADER_VISIBILITY visibility) {
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = filter;
	samplerDesc.AddressU = address_mode;
	samplerDesc.AddressV = address_mode;
	samplerDesc.AddressW = address_mode;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //比較しない
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                   //ありったけのMipmapを使う
	samplerDesc.ShaderRegister = shader_register;
	samplerDesc.ShaderVisibility = visibility;

	m_sampler_descs.emplace_back(samplerDesc);
}

bool RootSignature::Build(ID3D12Device* device) {
	if (m_root_parameters.empty() && m_sampler_descs.empty()) {
		LOG_ERROR("RootSignature::Build() - No parameters or samplers are registered.");
		return false;
	}

	//ルートシグネチャの全体設定を組み立てる
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionRootSignature.pParameters =
		m_root_parameters.empty() ? nullptr : m_root_parameters.data();
	descriptionRootSignature.NumParameters = static_cast<UINT>(m_root_parameters.size());
	descriptionRootSignature.pStaticSamplers =
		m_sampler_descs.empty() ? nullptr : m_sampler_descs.data();
	descriptionRootSignature.NumStaticSamplers = static_cast<UINT>(m_sampler_descs.size());

	//設定をもとにシリアライズ
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob
	);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to serialize RootSignature.");
		return false;
	}

	//バイナリをもとに作成
	hr = device->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_root_signature)
	);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to create RootSignature.");
		return false;
	}

	//ルートシグネチャが完成したので使用したコンテナの中身を空にする
	Clear();

	return true;
}

void RootSignature::Clear() {
	m_root_parameters.clear();
	m_root_parameters.shrink_to_fit();
	m_descriptor_ranges.clear();
	m_sampler_descs.clear();
	m_sampler_descs.shrink_to_fit();
}

