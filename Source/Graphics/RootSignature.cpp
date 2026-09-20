#include "Graphics/RootSignature.h"

#include "Debugger/Logger.h"

void RootSignature::AddCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility) {
	D3D12_ROOT_PARAMETER parameter{};
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	parameter.Descriptor.ShaderRegister = shaderRegister;
	parameter.Descriptor.RegisterSpace = 0;
	parameter.ShaderVisibility = visibility;
	//設定したデータを末尾に追加
	rootParameters_.emplace_back(parameter);
}

void RootSignature::AddDescriptorTable(const std::vector<D3D12_DESCRIPTOR_RANGE>& ranges, D3D12_SHADER_VISIBILITY visibility) {
	if (ranges.empty()) {
		return;
	}

	size_t startIndex = descriptorRanges_.size();
	for (const auto& range : ranges) {
		descriptorRanges_.push_back(range);
	}

	D3D12_ROOT_PARAMETER parameter{};
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	   //DescriptorTableを使う
	parameter.ShaderVisibility = visibility;
	parameter.DescriptorTable.pDescriptorRanges = &descriptorRanges_[startIndex];
	parameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(ranges.size());
	//設定したデータを末尾に追加
	rootParameters_.emplace_back(parameter);
}

void RootSignature::AddStaticSampler(UINT shaderRegister, D3D12_FILTER filter,
	D3D12_TEXTURE_ADDRESS_MODE addressMode, D3D12_SHADER_VISIBILITY visibility) {
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = filter;
	samplerDesc.AddressU = addressMode;
	samplerDesc.AddressV = addressMode;
	samplerDesc.AddressW = addressMode;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //比較しない
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                   //ありったけのMipmapを使う
	samplerDesc.ShaderRegister = shaderRegister;
	samplerDesc.ShaderVisibility = visibility;
	//設定したデータを末尾に追加
	samplerDescs_.emplace_back(samplerDesc);
}

bool RootSignature::Build(ID3D12Device* device) {
	if (rootParameters_.empty() && samplerDescs_.empty()) {
		LOG_ERROR("RootSignature::Build() - No parameters or samplers are registered.");
		return false;
	}

	//ルートシグネチャの全体設定を組み立てる
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionRootSignature.pParameters =
		rootParameters_.empty() ? nullptr : rootParameters_.data();
	descriptionRootSignature.NumParameters = static_cast<UINT>(rootParameters_.size());
	descriptionRootSignature.pStaticSamplers =
		samplerDescs_.empty() ? nullptr : samplerDescs_.data();
	descriptionRootSignature.NumStaticSamplers = static_cast<UINT>(samplerDescs_.size());

	//設定をもとにシリアライズ
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob
	);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to serialize RootSignature.");
		return false;
	}

	//バイナリをもとに作成
	hr = device->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_)
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
	rootParameters_.clear();
	rootParameters_.shrink_to_fit();
	descriptorRanges_.clear();
	samplerDescs_.clear();
	samplerDescs_.shrink_to_fit();
}

