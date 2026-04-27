#pragma once

#include <externals/DirectXTex/DirectXTex.h>

#include <cstdint>

#include <wrl/client.h>
#include <d3d12.h>

/**
 * \brief 定数バッファや頂点バッファなどの汎用バッファリソースを生成
 * \param[in] device DirectX12デバイス
 * \param[in] sizeInBytes 生成するリソースのサイズ(バイト単位)
 * \return 生成されたリソース
 */
Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

/**
 * \brief ディスクリプターヒープを生成
 * \param[in] device DirectX12デバイス
 * \param[in] heapType ヒープの種別
 * \param[in] numDescriptors 格納するディスクリプターの数
 * \param[in] shaderVisible シェーダーから参照可能にするか
 * \return 生成されたディスクリプターヒープ
 */
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

/**
 * \brief テクスチャリソースを生成
 * \param[in] device DirectX12デバイス
 * \param[in] metadata TexMetadata
 * \return 生成されたテクスチャリソース
 */
Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

/**
 * \brief 深度ステンシル用のテクスチャリソースを生成
 * \param[in] device DirectX12デバイス
 * \param[in] width テクスチャの横幅
 * \param[in] height テクスチャの横幅
 * \return 生成された深度ステンシルリソース
 */
Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);




