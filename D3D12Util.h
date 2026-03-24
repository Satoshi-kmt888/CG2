#pragma once

struct ID3D12Resource;
struct ID3D12Device;

//バッファリソースの生成
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);
