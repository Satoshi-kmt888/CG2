#include "DescriptorManager.h"

#include <cassert>

DescriptorManager* DescriptorManager::GetInstance() {
	static DescriptorManager instance;
	return &instance;
}

void DescriptorManager::Initialize(ID3D12Device* device) {
	assert(device);

	//各ヒープを初期化
	for (size_t i = 0; i < pools_.size(); ++i) {
		auto type = static_cast<DescriptorType>(i);
		HeapPool& pool = pools_[i];

		D3D12_DESCRIPTOR_HEAP_DESC desc{};

		switch (type) {
		case DescriptorType::SRV_CBV_UAV:
			pool.maxDescriptors = kMaxSrvCbvUavDescriptors;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			pool.descriptorSize = device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
			);
			break;

		case DescriptorType::RTV:
			pool.maxDescriptors = kMaxRtvDescriptors;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			pool.descriptorSize = device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_RTV
			);
			break;

		case DescriptorType::DSV:
			pool.maxDescriptors = kMaxDsvDescriptors;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			pool.descriptorSize = device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_DSV
			);
			break;

		default:
			break;
		}

		desc.NumDescriptors = pool.maxDescriptors;

		HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pool.descriptorHeap));
		assert(SUCCEEDED(hr));
	}
}

DescriptorHandle DescriptorManager::Allocate(DescriptorType type) {
	HeapPool& pool = pools_[static_cast<size_t>(type)];
	uint32_t assignedIndex = 0;

	//インデックスが使用可能であれば追加
	//そうでなければ次のインデックスを使用
	if (!pool.freeIndices.empty()) {
		assignedIndex = pool.freeIndices.back();
		pool.freeIndices.pop_back();
	} else {
		assert(pool.nextIndex < pool.maxDescriptors);
		assignedIndex = pool.nextIndex;
		pool.nextIndex++;
	}

	DescriptorHandle handle;
	handle.index = assignedIndex;
	handle.type = type;

	//CPUハンドルの計算
	handle.cpuHandle = pool.descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.cpuHandle.ptr += (static_cast<size_t>(pool.descriptorSize) * assignedIndex);

	//GPUハンドルの計算(SRV_CBV_UAVのみ)
	if (type == DescriptorType::SRV_CBV_UAV) {
		handle.gpuHandle = pool.descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handle.gpuHandle.ptr += (static_cast<size_t>(pool.descriptorSize) * assignedIndex);
	}

	return handle;
}

void DescriptorManager::Free(const DescriptorHandle& handle) {
	HeapPool& pool = pools_[static_cast<size_t>(handle.type)];
	pool.freeIndices.push_back(handle.index);
}

void DescriptorManager::Finalize() {
	for (auto& pool : pools_) {
		pool.descriptorHeap.Reset();
		pool.freeIndices.clear();
		pool.nextIndex = 0;
	}
}
