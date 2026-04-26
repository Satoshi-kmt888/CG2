#include <d3d12.h>
#include "TextureLoader.h"

#include "D3D12Util.h"
#include "DirectXCommon.h"
#include "StringUtil.h"

#include <externals/DirectXTex/d3dx12.h>
#include <externals/DirectXTex/DirectXTex.h>

#include <cassert>
#include <string>
#include <vector>

#include <Windows.h>

DirectX::ScratchImage LoadTexture(const std::string& filePath) {
	//テクスチャを読み込んで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);

	return mipImages;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	//中間リソースを作る
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
	Microsoft::WRL::ComPtr<ID3D12Device> device = DirectXCommon::GetInstance()->GetDevice();
	HRESULT hr = DirectX::PrepareUpload(
		device.Get(),
		mipImages.GetImages(),
		mipImages.GetImageCount(),
		mipImages.GetMetadata(),
		subResources
	);
	assert(SUCCEEDED(hr));
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subResources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device.Get(), intermediateSize);

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = DirectXCommon::GetInstance()->GetCommandList();
	//データ転送をコマンドに積む
	UpdateSubresources(
		commandList.Get(),
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
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}
