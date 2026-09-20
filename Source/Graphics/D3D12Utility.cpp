#include "Graphics/D3D12Utility.h"

#include <cassert>
#include <dxgiformat.h>
#include <Windows.h>

namespace D3D12Utility {
	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
		//リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

		//頂点リソースの設定
		D3D12_RESOURCE_DESC vertexResourceDesc{};
		//バッファリソース。テクスチャの場合はまた別の設定をする
		vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vertexResourceDesc.Width = sizeInBytes; //リソースサイズ
		//バッファの場合はこれらは1にする決まり
		vertexResourceDesc.Height = 1;
		vertexResourceDesc.DepthOrArraySize = 1;
		vertexResourceDesc.MipLevels = 1;
		vertexResourceDesc.SampleDesc.Count = 1;
		//バッファにする場合はこれにする決まり
		vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//実際にリソースを作る
		ComPtr<ID3D12Resource> resource = nullptr;
		if (HRESULT hr = device->CreateCommittedResource(
			&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource)
		); FAILED(hr)) {
			assert(SUCCEEDED(hr));
			return nullptr;
		}

		return resource;
	}

	ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
		//metadataをもとにResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = UINT(metadata.width); //Textureの幅
		resourceDesc.Height = UINT(metadata.height); //Textureの高さ
		resourceDesc.MipLevels = UINT16(metadata.mipLevels); //mipmapの数
		resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); //奥行きor配列Textureの配列数
		resourceDesc.Format = metadata.format; //Textureのformat
		resourceDesc.SampleDesc.Count = 1; //サンプリングカウント。1固定
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); //Textureでの次元数

		//利用するHeapの設定。非常に特殊な運用。
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //細かい設定を行う

		//Resourceの生成
		ComPtr<ID3D12Resource> resource = nullptr;

		if (HRESULT hr = device->CreateCommittedResource(
			&heapProperties, //Heapの設定
			D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定
			&resourceDesc, //Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST, //初回のResourceState
			nullptr,
			IID_PPV_ARGS(&resource)
		); FAILED(hr)) {
			assert(SUCCEEDED(hr));
			return nullptr;
		}

		return resource;
	}

	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
		//生成するResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = width; //Textureの幅
		resourceDesc.Height = height; //Textureの高さ
		resourceDesc.MipLevels = 1; //mipmapの数
		resourceDesc.DepthOrArraySize = 1; //奥行き or 配列Textureの配列数
		resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DepthStencilとして利用可能なフォーマット
		resourceDesc.SampleDesc.Count = 1; //サンプリングカウント。1固定。
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //2次元
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //DepthStencilとpして使う通知

		//利用するHeapの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //VRAM上に作る

		//深度値のクリア設定
		D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.DepthStencil.Depth = 1.0f; //最大値でクリア
		depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //フォーマット。Resourceと合わせる

		//Resourceの生成
		ComPtr<ID3D12Resource> resource = nullptr;
		if (HRESULT hr = device->CreateCommittedResource(
			&heapProperties, //Heapの設定
			D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし。
			&resourceDesc, //Resourceの設定
			D3D12_RESOURCE_STATE_DEPTH_WRITE, //深度値を書き込む状態にしておく
			&depthClearValue, //Clear最適値
			IID_PPV_ARGS(&resource)
		); FAILED(hr)) {
			assert(SUCCEEDED(hr));
			return nullptr;
		}

		return resource;
	}
} //namespace D3D12Utility
