#include "CommandContext.h"

#include "Debugger/Logger.h"

#include <array>

CommandContext::CommandContext() = default;

CommandContext::~CommandContext() {
	Finalize();
}

bool CommandContext::Initialize(ID3D12Device* device) {
	if (m_initialized) {
		LOG_INFO("既に初期化済みです。");
		return false;
	}

	if (!device) {
		LOG_ERROR("引数がnullptrのため、CommandContextを初期化できません。");
		return false;
	}

	if (!CreateCommandObjects(device)) {
		Finalize();
		return false;
	}

	if (!CreateFence(device)) {
		Finalize();
		return false;
	}

	m_initialized = true;
	LOG_INFO("CommandContextの初期化が正常に完了しました。");
	return true;
}

bool CommandContext::Reset() const {
	if (!m_initialized) {
		LOG_ERROR("初期化が完了していません。初期化関数を実行してください。");
		return false;
	}

	if (FAILED(m_commandAllocator->Reset())) {
		LOG_ERROR("CommandAllocatorのリセットに失敗しました。");
		return false;
	}

	//コマンドリストのリセット
	if (FAILED(m_commandList->Reset(m_commandAllocator.Get(), nullptr))) {
		LOG_ERROR("CommandListのリセットに失敗しました。");
		return false;
	}

	return true;
}

bool CommandContext::Execute() const {
	if (!m_initialized) {
		LOG_ERROR("初期化が完了していません。初期化関数を実行してください。");
		return false;
	}

	if (FAILED(m_commandList->Close())) {
		LOG_ERROR("CommandListをCloseすることに失敗しました(不正なコマンドが記録されている可能性があります)。");
		return false;
	}

	//キューへ実行を要求
	std::array<ID3D12CommandList*, 1> commandLists = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());

	return true;
}

bool CommandContext::WaitForGPU() {
	//次のフェンス値が有効かチェックしてからフェンス値を進めるようにする
	const uint64_t nextValue = m_fenceValue + 1;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	if (FAILED(m_commandQueue->Signal(m_fence.Get(), nextValue))) {
		LOG_ERROR("CommandQueue への Signal 発行に失敗しました。");
		return false;
	}
	m_fenceValue = nextValue;

	//Fenceの値が指定したSignal値にたどり着いているかを確認する
	if (m_fence->GetCompletedValue() < m_fenceValue) {
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		if (SUCCEEDED(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent))) {
			//イベントを待つ
			WaitForSingleObject(m_fenceEvent, INFINITE);
		} else {
			LOG_ERROR("SetEventOnCompletion の設定に失敗しました。");
			return false;
		}
	}

	return true;
}

void CommandContext::Finalize() {
	if (m_commandQueue && m_fence && m_fenceEvent) {
		WaitForGPU();
	}

	if (m_fenceEvent) {
		CloseHandle(m_fenceEvent);
		m_fenceEvent = nullptr;
	}

	m_fence.Reset();
	m_commandList.Reset();
	m_commandAllocator.Reset();
	m_commandQueue.Reset();

	m_initialized = false;
}

bool CommandContext::CreateCommandObjects(ID3D12Device* device) {
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;          //登録可能なコマンドリストのタイプ
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; //コマンドキューの優先度
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;          //特性を設定するフラグ
	commandQueueDesc.NodeMask = 0;                                   //GPUが一つであれば0

	//コマンドキュー
	if (FAILED(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_commandQueue)))) {
		LOG_ERROR("CommandQueue の生成に失敗しました。");
		return false;
	}

	//コマンドアロケータ
	if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)))) {
		LOG_ERROR("CommandAllocator の生成に失敗しました。");
		return false;
	}

	//コマンドリスト
	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)))) {
		LOG_ERROR("CommandList の生成に失敗しました。");
		return false;
	}

	return true;
}

bool CommandContext::CreateFence(ID3D12Device* device) {
	if (FAILED(device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)))) {
		LOG_ERROR("Fence の生成に失敗しました。");
		return false;
	}

	//FenceのSignalを待つためのイベントを作成する
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) {
		LOG_ERROR("Fence 用のWin32イベント作成に失敗しました。");
		return false;
	}

	return true;
}
