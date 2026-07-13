#pragma once

#include <cstdint>
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include <array>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// ディスクリプタの種類
/// </summary>
enum class DescriptorType {
	SRV_CBV_UAV, //テクスチャ、定数バッファなど
	RTV,         //レンダーターゲット
	DSV,         //深度ステンシル

	Count        //種類の総数
};

/// <summary>
/// ディスクリプタ
/// </summary>
struct DescriptorHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
	uint32_t index = 0;
	DescriptorType type = DescriptorType::SRV_CBV_UAV;
};

/// <summary>
/// ディスクリプター
/// </summary>
class DescriptorManager {
public:
	//--- 公開定数 ---

	static constexpr uint32_t kMaxSrvCbvUavDescriptors = 2048;
	static constexpr uint32_t kMaxRtvDescriptors = 64;
	static constexpr uint32_t kMaxDsvDescriptors = 64;

	//--- インスタンス管理 ---

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static DescriptorManager* GetInstance();

	//コピーガード
	DescriptorManager(const DescriptorManager&) = delete;
	DescriptorManager& operator=(const DescriptorManager&) = delete;

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
	DescriptorHandle Allocate(DescriptorType type);

	/// <summary>
	/// ディスクリプタの解放 / 再利用リストへの返却
	/// </summary>
	/// <param name="handle"></param>
	void Free(const DescriptorHandle& handle);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	//--- ゲッター ---

	ID3D12DescriptorHeap* GetHeap(DescriptorType type) const { return pools_[static_cast<size_t>(type)].descriptorHeap.Get(); }

private:
	//--- インスタンス管理 ---

	DescriptorManager() = default;
	~DescriptorManager() = default;

	//--- 内部データ構造体 ---

	//ヒープ1つ分の管理データ
	struct HeapPool {
		ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
		uint32_t descriptorSize = 0;
		uint32_t nextIndex = 0;
		uint32_t maxDescriptors = 0;
		std::vector<uint32_t> freeIndices;
	};

	//--- 内部変数 ---

	//ヒーププール
	std::array<HeapPool, static_cast<size_t>(DescriptorType::Count)> pools_;
};
