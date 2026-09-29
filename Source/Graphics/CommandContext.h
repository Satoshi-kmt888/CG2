#pragma once

#include <cstdint>
#include <d3d12.h>
#include <Windows.h>
#include <wrl/client.h>

/// <summary>
/// コマンドの発行と実行を行うクラス。
/// </summary>
class CommandContext {
public:
	//==================================================
	// public methods
	//==================================================

	CommandContext();
	~CommandContext();

	//コピー・ムーブ禁止
	CommandContext(const CommandContext&) = delete;
	CommandContext& operator=(const CommandContext&) = delete;
	CommandContext(CommandContext&&) = delete;
	CommandContext& operator=(CommandContext&&) = delete;

	/// <summary>
	/// コマンドおよび同期用リソースの初期化
	/// </summary>
	/// <returns>初期化成功時にtrue</returns>
	bool Initialize(ID3D12Device* device);

	/// <summary>
	/// コマンドアロケータとコマンドリストを次のフレーム用にリセットする
	/// </summary>
	/// <returns>リセット成功時にtrue</returns>
	bool Reset() const;

	/// <summary>
	/// コマンドリストの記録を確定させ、コマンドキューに実行をリクエストする
	/// </summary>
	/// <returns>実行リクエスト成功時にtrue</returns>
	bool Execute() const;

	/// <summary>
	/// GPUが現在のフェンス位置に到達するまでCPUをブロックして待機する
	/// </summary>
	/// <returns></returns>
	bool WaitForGPU();

	/// <summary>
	/// コマンドおよび同期用リソースの終了・解放処理
	/// </summary>
	void Finalize();

	ID3D12CommandQueue* GetCommandQueue() const { return m_commandQueue.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }

private:
	//==================================================
	// private methods
	//==================================================

	/// <summary>
	/// キュー / アロケータ / リストの各コマンドを生成する
	/// </summary>
	/// <returns>生成成功時にtrue</returns>
	bool CreateCommandObjects(ID3D12Device* device);

	/// <summary>
	/// 同期用のフェンスおよびイベントハンドルを生成する
	/// </summary>
	/// <returns>生成成功時にtrue</returns>
	bool CreateFence(ID3D12Device* device);

	//==================================================
	// private variables
	//==================================================

	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	bool m_initialized = false;

	//コマンド関連
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;

	//同期用フェンス
	ComPtr<ID3D12Fence> m_fence;
	uint64_t m_fenceValue = 0;
	HANDLE m_fenceEvent = nullptr;
};
