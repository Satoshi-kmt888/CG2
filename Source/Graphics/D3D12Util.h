#pragma once
#include <d3d12.h>
#include <Windows.h>
#include <wrl/client.h>

#include <cstdint>

#include <DirectXTex.h>

/**
 * \brief 定数バッファや頂点バッファなどの汎用バッファリソースを生成
 * \param[in] device DirectX12デバイス
 * \param[in] sizeInBytes 生成するリソースのサイズ(バイト単位)
 * \return 生成されたリソース
 */
Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

/**
 * \brief テクスチャリソースを生成
 * \param[in] device DirectX12デバイス
 * \param[in] metadata TexMetadata
 * \return 生成されたテクスチャリソース
 */
Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

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
 * \brief 深度ステンシル用のテクスチャリソースを生成
 * \param[in] device DirectX12デバイス
 * \param[in] width テクスチャの横幅
 * \param[in] height テクスチャの横幅
 * \return 生成された深度ステンシルリソース
 */
Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

/**
 * \brief ディスクリプターヒープから指定インデックスのCPUハンドルを取得する
 * \param[in] descriptorHeap 対象のディスクリプターヒープ
 * \param[in] descriptorSize ディスクリプタ一つ分のサイズ(デバイスから取得)
 * \param[in] index 取得したい場所のインデックス
 * \return 算出されたCPUハンドル
 */
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap,
	uint32_t descriptorSize,
	uint32_t index
);

/**
 * \brief ディスクリプターヒープから指定インデックスのGPUハンドルを取得する
 * \param[in] descriptorHeap 対象のディスクリプターヒープ
 * \param[in] descriptorSize ディスクリプタ一つ分のサイズ(デバイスから取得)
 * \param[in] index 取得したい場所のインデックス
 * \return 算出されたGPUハンドル
 */
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap,
	uint32_t descriptorSize,
	uint32_t index
);
