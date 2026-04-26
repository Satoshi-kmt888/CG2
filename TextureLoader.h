#pragma once

#include <string>

#include <externals/DirectXTex/DirectXTex.h>

#include <wrl/client.h>

struct ID3D12Resource;

//テクスチャの読み込み
DirectX::ScratchImage LoadTexture(const std::string& filePath);

//テクスチャデータの更新
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
