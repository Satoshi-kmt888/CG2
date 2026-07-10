#include "TextureManager.h"

#include "Graphics/D3D12Utility.h"
#include "Diagnostics/Logger.h"
#include "Core/StringUtility.h"
#include "GraphicsSystem.h"

#include <d3dx12.h>
#include <DirectXTex.h>

#include <cassert>
#include <cstdint>
#include <format>
#include <string>
#include <utility>
#include <vector>

#include <d3d12.h>
#include <Windows.h>
#include <wrl/client.h>

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
	//メンバ変数に引数のデータを代入
	device_ = device;
	commandList_ = commandList;

	srvDescriptorHeap_ = D3D12Utility::CreateDescriptorHeap(
		device_,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		kMaxTextures,
		true
	);

	srvDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//先頭はImGuiが使用
	srvDescriptorIndex_ = 1;
}

void TextureManager::Finalize() {
	intermediateResource_.clear();
	textureDataMap_.clear();

	srvDescriptorHeap_.Reset();
}

const TextureData& TextureManager::Load(const std::string& filePath) {
	//既に読み込み済みならそれを返す
	if (textureDataMap_.contains(filePath)) {
		return textureDataMap_[filePath];
	}

	assert(srvDescriptorIndex_ < kMaxTextures && "テクスチャの最大数を超えました");

	DirectX::ScratchImage mipImages = ReadFile(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	TextureData data;
	data.metadata = metadata;
	data.resource = D3D12Utility::CreateTextureResource(device_, metadata);

	intermediateResource_.push_back(UploadTextureData(data.resource.Get(), mipImages));

	//SRVを作成するDescriptorHeapの場所を決める
	data.cpuHandle = D3D12Utility::GetCPUDescriptorHandle(srvDescriptorHeap_.Get(), srvDescriptorSize_, srvDescriptorIndex_);
	data.gpuHandle = D3D12Utility::GetGPUDescriptorHandle(srvDescriptorHeap_.Get(), srvDescriptorSize_, srvDescriptorIndex_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);
	//SRVの生成
	device_->CreateShaderResourceView(data.resource.Get(), &srvDesc, data.cpuHandle);
	//次のテクスチャ用にカウントを進める
	srvDescriptorIndex_++;

	textureDataMap_[filePath] = std::move(data);
	return textureDataMap_[filePath];
}

DirectX::ScratchImage TextureManager::ReadFile(const std::string& filePath) {
	//テクスチャを読み込んで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);

	return mipImages;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	//中間リソースを作る
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
	HRESULT hr = DirectX::PrepareUpload(
		device_,
		mipImages.GetImages(),
		mipImages.GetImageCount(),
		mipImages.GetMetadata(),
		subResources
	);
	assert(SUCCEEDED(hr));
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subResources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = D3D12Utility::CreateBufferResource(device_, intermediateSize);

	//データ転送をコマンドに積む
	UpdateSubresources(
		commandList_,
		texture,
		intermediateResource.Get(),
		0,
		0,
		UINT(subResources.size()),
		subResources.data()
	);

	//ResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList_->ResourceBarrier(1, &barrier);

	return intermediateResource;
}
