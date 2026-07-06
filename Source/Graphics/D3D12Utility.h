#pragma once

#include <cstdint>
#include <d3d12.h>
#include <Windows.h>
#include <wrl/client.h>
#include <DirectXTex.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace D3D12Utility {
	/// <summary>
	/// 定数バッファや頂点バッファなどの汎用バッファリソースを生成
	/// </summary>
	/// <param name="device">DirectX12デバイス</param>
	/// <param name="sizeInBytes">生成するリソースのサイズ(バイト単位)</param>
	/// <returns>生成されたリソース</returns>
	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="device"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	/// <summary>
	/// ディスクリプターヒープを生成
	/// </summary>
	/// <param name="device">DirectX12デバイス</param>
	/// <param name="heapType">ヒープの種別</param>
	/// <param name="numDescriptors">格納するディスクリプターの数</param>
	/// <param name="shaderVisible">シェーダーから参照可能にするか</param>
	/// <returns>生成されたディスクリプターヒープ</returns>
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	/// <summary>
	/// 深度ステンシル用のテクスチャリソースを生成
	/// </summary>
	/// <param name="device">DirectX12デバイス</param>
	/// <param name="width">テクスチャの横幅</param>
	/// <param name="height">テクスチャの横幅</param>
	/// <returns>生成された深度ステンシルリソース</returns>
	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

	/// <summary>
	/// ディスクリプターヒープから指定インデックスのCPUハンドルを取得する
	/// </summary>
	/// <param name="descriptorHeap">対象のディスクリプターヒープ</param>
	/// <param name="descriptorSize">ディスクリプタ一つ分のサイズ(デバイスから取得)</param>
	/// <param name="index">取得したい場所のインデックス</param>
	/// <returns>算出されたCPUハンドル</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		ID3D12DescriptorHeap* descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index
	);

	/// <summary>
	/// ディスクリプターヒープから指定インデックスのGPUハンドルを取得する
	/// </summary>
	/// <param name="descriptorHeap">対象のディスクリプターヒープ</param>
	/// <param name="descriptorSize">ディスクリプタ一つ分のサイズ(デバイスから取得)</param>
	/// <param name="index">取得したい場所のインデックス</param>
	/// <returns>算出されたGPUハンドル</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		ID3D12DescriptorHeap* descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index
	);
} //namespace D3D12Utility
