#include "DescriptorManager.h"

#include "Graphics/D3D12Utility.h"

#include <cassert>

DescriptorManager* DescriptorManager::GetInstance() {
	static DescriptorManager instance;
	return &instance;
}

void DescriptorManager::Initialize(ID3D12Device* device) {
	assert(device);

	descriptorHeap_ = D3D12Utility::CreateDescriptorHeap(
		device,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		kMaxDescriptors,
		true
	);

	descriptorSize_ = device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);

	nextIndex_ = 0;
	freeIndices_.clear();
}

DescriptorHandle DescriptorManager::Allocate() {
	uint32_t assignedIndex = 0;

	if (!freeIndices_.empty()) {
		assignedIndex = freeIndices_.back();
		freeIndices_.pop_back();
	} else {
		assert(nextIndex_ < kMaxDescriptors);
		assignedIndex = nextIndex_;
		nextIndex_++;
	}

	DescriptorHandle handle;
	handle.index = assignedIndex;
	handle.cpuHandle = D3D12Utility::GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, assignedIndex);
	handle.gpuHandle = D3D12Utility::GetGPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, assignedIndex);

	return handle;
}

void DescriptorManager::Free(const DescriptorHandle& handle) {
	freeIndices_.push_back(handle.index);
}

void DescriptorManager::Finalize() {
	descriptorHeap_.Reset();
	freeIndices_.clear();
}
