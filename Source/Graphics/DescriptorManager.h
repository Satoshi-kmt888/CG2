#pragma once

#include <cstdint>
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// 
/// </summary>
struct DescriptorHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
	uint32_t index = 0;
};

/// <summary>
/// ディスクリプター
/// </summary>
class DescriptorManager {
public:
	//--- 公開定数 ---

	//総容量
	static const uint32_t kMaxDescriptors = 256;

	//--- インスタンス管理 ---

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static DescriptorManager* GetInstance();

	DescriptorManager() = default;
	~DescriptorManager() = default;

	//--- 公開関数 ---

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// ディスクリプタの割り当て
	/// </summary>
	/// <returns></returns>
	DescriptorHandle Allocate();

	void Free(const DescriptorHandle& handle);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	//--- ゲッター ---

	ID3D12DescriptorHeap* GetHeap() const { return descriptorHeap_.Get(); }

private:
	//--- 内部変数 ---

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
	uint32_t descriptorSize_ = 0;
	uint32_t nextIndex_ = 0;
	std::vector<uint32_t> freeIndices_;
};

