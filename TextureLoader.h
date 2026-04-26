#pragma once
#include <string>

#include <externals/DirectXTex/DirectXTex.h>

#include <d3d12.h>

//テクスチャの読み込み
DirectX::ScratchImage LoadTexture(const std::string& filePath);

//テクスチャデータの更新
void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
