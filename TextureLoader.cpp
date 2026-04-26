#include "TextureLoader.h"

#include "StringUtil.h"

#include <externals/DirectXTex/DirectXTex.h>

#include <cassert>
#include <string>

#include <d3d12.h>
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

void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全mipMap
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		//MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,			  //全領域へコピー 
			img->pixels,		  //元データアドレス
			UINT(img->rowPitch),  //1ラインサイズ
			UINT(img->slicePitch) //1枚のサイズ
		);
		assert(SUCCEEDED(hr));
	}
}
