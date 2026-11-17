#include "TextureManager.h"

#include "App/StringUtility.h"
#include "Debugger/Logger.h"
#include "Graphics/D3D12Utility.h"

#include <cassert>
#include <cstdint>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXTex.h>
#include <format>
#include <string>
#include <utility>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
	//メンバ変数に引数のデータを代入
	m_device = device;
	m_command_list = commandList;
}

void TextureManager::Finalize() {
	intermediate_resource.clear();
	m_data_map.clear();
}

const TextureData& TextureManager::Load(const std::string& filePath) {
	//既に読み込み済みならそれを返す
	if (m_data_map.contains(filePath)) {
		return m_data_map[filePath];
	}

	//画像ファイルの読み込み
	DirectX::ScratchImage mip_images = ReadFile(filePath);
	const DirectX::TexMetadata& metadata = mip_images.GetMetadata();

	//リソースの生成とVRAM転送
	TextureData data;
	data.metadata = metadata;
	data.resource = D3D12Utility::CreateTextureResource(m_device, metadata);

	intermediate_resource.push_back(UploadTextureData(data.resource.Get(), mip_images));

	data.descriptor_handle = DescriptorManager::GetInstance()->Allocate(DescriptorType::SRV_CBV_UAV);

	//SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Format = metadata.format;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);

	//SRVの生成
	m_device->CreateShaderResourceView(data.resource.Get(), &srv_desc, data.descriptor_handle.cpu_handle);

	m_data_map[filePath] = std::move(data);
	return m_data_map[filePath];
}

DirectX::ScratchImage TextureManager::ReadFile(const std::string& filePath) {
	//テクスチャを読み込んで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring file_path_w = StringUtility::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(file_path_w.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	const auto& metadata = image.GetMetadata();
	if (metadata.width <= 1 && metadata.height <= 1) {
		return image; // そのまま返す
	}

	//ミップマップの作成
	DirectX::ScratchImage mip_images{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mip_images);
	assert(SUCCEEDED(hr));

	return mip_images;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	//中間リソースを作る
	std::vector<D3D12_SUBRESOURCE_DATA> sub_resources;
	HRESULT hr = DirectX::PrepareUpload(
		m_device,
		mipImages.GetImages(),
		mipImages.GetImageCount(),
		mipImages.GetMetadata(),
		sub_resources
	);
	assert(SUCCEEDED(hr));
	uint64_t intermediate_size = GetRequiredIntermediateSize(texture, 0, UINT(sub_resources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediate_resource = D3D12Utility::CreateBufferResource(m_device, intermediate_size);

	//データ転送をコマンドに積む
	UpdateSubresources(
		m_command_list,
		texture,
		intermediate_resource.Get(),
		0,
		0,
		UINT(sub_resources.size()),
		sub_resources.data()
	);

	//ResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	m_command_list->ResourceBarrier(1, &barrier);

	return intermediate_resource;
}
