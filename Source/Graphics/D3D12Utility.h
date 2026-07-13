#pragma once

#include <cstdint>
#include <d3d12.h>
#include <DirectXTex.h>
#include <wrl/client.h>

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
	/// テクスチャリソースを生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	/// <summary>
	/// 深度ステンシル用のテクスチャリソースを生成
	/// </summary>
	/// <param name="device">DirectX12デバイス</param>
	/// <param name="width">テクスチャの横幅</param>
	/// <param name="height">テクスチャの横幅</param>
	/// <returns>生成された深度ステンシルリソース</returns>
	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);
} //namespace D3D12Utility
