#pragma once

#include <externals/DirectXTex/DirectXTex.h>

struct ID3D12Resource;
struct ID3D12Device;

//バッファリソースの生成
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

//ディスクリプターヒープの作成
ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

//テクスチャリソースの生成
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
