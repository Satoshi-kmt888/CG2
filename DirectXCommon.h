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
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	ID3D12Resource* swapChainResources[2] = { nullptr };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent = nullptr;

	uint32_t backBufferIndex = 0;
};
