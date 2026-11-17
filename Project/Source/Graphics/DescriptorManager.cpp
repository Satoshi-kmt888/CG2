#include "DescriptorManager.h"

#include <cassert>

DescriptorManager* DescriptorManager::GetInstance() {
	static DescriptorManager instance;
	return &instance;
}

void DescriptorManager::Initialize(ID3D12Device* device) {
	assert(device);

	//各ヒープを初期化
	for (size_t i = 0; i < m_pools.size(); ++i) {
		auto type = static_cast<DescriptorType>(i);
		HeapPool& pool = m_pools[i];

		D3D12_DESCRIPTOR_HEAP_DESC desc{};

		switch (type) {
		case DescriptorType::SRV_CBV_UAV:
			pool.maxDescriptors = k_max_srv_cbv_uav_descriptors;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			pool.descriptorSize = device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
			);
			break;

		case DescriptorType::RTV:
			pool.maxDescriptors = k_max_rtv_descriptors;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			pool.descriptorSize = device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_RTV
			);
			break;

		case DescriptorType::DSV:
			pool.maxDescriptors = k_max_dsv_descriptors;
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
	HeapPool& pool = m_pools[static_cast<size_t>(type)];
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
	handle.cpu_handle = pool.descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.cpu_handle.ptr += (static_cast<size_t>(pool.descriptorSize) * assignedIndex);

	//GPUハンドルの計算(SRV_CBV_UAVのみ)
	if (type == DescriptorType::SRV_CBV_UAV) {
		handle.gpu_handle = pool.descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handle.gpu_handle.ptr += (static_cast<size_t>(pool.descriptorSize) * assignedIndex);
	}

	return handle;
}

void DescriptorManager::Free(const DescriptorHandle& handle) {
	HeapPool& pool = m_pools[static_cast<size_t>(handle.type)];
	pool.freeIndices.push_back(handle.index);
}

void DescriptorManager::Finalize() {
	for (auto& pool : m_pools) {
		pool.descriptorHeap.Reset();
		pool.freeIndices.clear();
		pool.nextIndex = 0;
	}
}
