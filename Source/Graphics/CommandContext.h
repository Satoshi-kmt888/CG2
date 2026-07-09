#pragma once

#include <cstdint>
#include <d3d12.h>
#include <Windows.h>
#include <wrl/client.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class GraphicsDevice;

/// <summary>
/// コマンド発行・実行およびGPU同期を管理するクラス
/// </summary>
class CommandContext {
public:
	//--- インスタンス管理 ---

	CommandContext() = default;
	~CommandContext();

	//コピーガード
	CommandContext(const CommandContext&) = delete;
	CommandContext& operator=(const CommandContext&) = delete;

	//--- 公開関数 ---

	/// <summary>
	/// コマンドおよび同期用リソースの初期化
	/// </summary>
	/// <param name="graphicsDevice">グラフィックスデバイスへのポインタ</param>
	/// <returns>初期化成功時にtrue</returns>
	bool Initialize(const GraphicsDevice* graphicsDevice);

	/// <summary>
	/// コマンドアロケータとコマンドリストを次のフレーム用にリセットする
	/// </summary>
	/// <returns>リセット成功時にtrue</returns>
	bool Reset();

	/// <summary>
	/// コマンドリストの記録を確定させ、コマンドキューに実行をリクエストする
	/// </summary>
	/// <returns>実行リクエスト成功時にtrue</returns>
	bool Execute();

	/// <summary>
	/// GPUが現在のフェンス位置に到達するまでCPUをブロックして待機する
	/// </summary>
	void WaitForGPU();

	/// <summary>
	/// コマンドおよび同期用リソースの終了・解放処理
	/// </summary>
	void Finalize();

	//--- ゲッター ---

	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

private:
	//--- 内部関数 ---

	/// <summary>
	/// キュー / アロケータ / リストの各コマンドを生成する
	/// </summary>
	/// <param name="device">DirectX12のデバイスオブジェクトへのポインタ</param>
	/// <returns>生成成功時にtrue</returns>
	bool CreateCommand(ID3D12Device* device);

	/// <summary>
	/// 同期用のフェンスおよびイベントハンドルを生成する
	/// </summary>
	/// <param name="device">DirectX12のデバイスオブジェクトへのポインタ</param>
	/// <returns>生成成功時にtrue</returns>
	bool CreateFence(ID3D12Device* device);

	//--- 内部変数 ---

	//コマンド関連
	ComPtr<ID3D12CommandQueue> commandQueue_;
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	ComPtr<ID3D12GraphicsCommandList> commandList_;

	//同期用フェンス
	ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_ = nullptr;
};
