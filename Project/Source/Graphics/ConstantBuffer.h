#pragma once

#include "Debugger/Logger.h"
#include "Graphics/D3D12Utility.h"

#include <d3d12.h>
#include <wrl/client.h>

template<typename T>
class ConstantBuffer {
public:
	//=========================================================
	// public methods
	//=========================================================

	ConstantBuffer() = default;
	~ConstantBuffer();

	//コピー禁止
	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;

	bool Create(ID3D12Device* device);

	T* operator->() { return m_data; }
	T& Data() { return *m_data; }
	const T* operator->() const { return m_data; }
	const T& Data() const { return *m_data; }

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return m_resource->GetGPUVirtualAddress(); }

private:
	//=========================================================
	// private variables
	//=========================================================

	Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
	T* m_data = nullptr;
};

template<typename T>
inline ConstantBuffer<T>::~ConstantBuffer() {
	if (m_resource && m_data) {
		m_resource->Unmap(0, nullptr);
		m_data = nullptr;
	}
}

template<typename T>
inline bool ConstantBuffer<T>::Create(ID3D12Device* device) {
	if (!device) {
		LOG_ERROR("引数deviceがnullptrです。");
		return false;
	}

	m_resource = D3D12Utility::CreateBufferResource(device, sizeof(T));
	if (!m_resource) {
		LOG_ERROR("リソースの生成に失敗しました。");
		return false;
	}

	void* resource_ptr = nullptr;
	if (FAILED(m_resource->Map(0, nullptr, &resource_ptr))) {
		LOG_ERROR("リソースのマッピングに失敗しました。");
		m_resource.Reset();
		return false;
	}

	m_data = static_cast<T*>(resource_ptr);
	*m_data = T{};

	return true;
}
