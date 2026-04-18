#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>

#include "WinApp.h"

/// <summary>
/// DirectXの基盤
/// </summary>
class DirectXCommon {
public:

	void Initialize(WinApp* winApp);
	void PreDraw();
	void PostDraw();
	void Finalize();

	ID3D12Device* GetDevice() const { return device; }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList; }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc; }
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() const { return rtvDesc; }
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return srvDescriptorHeap; }

private:
	void CreateDevice();
	void CreateCommand();
	void CreateSwapChain(WinApp* winApp);
	void CreateFinalRenderTargets();
	void CreateFence();

private:
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGIAdapter4* useAdapter = nullptr;
	ID3D12Device* device = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	ID3D12Resource* swapChainResources[2] = { nullptr };
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	ID3D12DescriptorHeap* srvDescriptorHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent = nullptr;

	uint32_t backBufferIndex = 0;
};
