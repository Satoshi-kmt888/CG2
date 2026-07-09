#include "CommandContext.h"

#include "GraphicsDevice.h"
#include "Diagnostics/Logger.h"

#include <array>

CommandContext::~CommandContext() {
	Finalize();
}

bool CommandContext::Initialize(const GraphicsDevice* graphicsDevice) {
	if (graphicsDevice == nullptr) {
		LOG_ERROR("GraphicsDevice が nullptr のため、CommandContext を初期化できません。");
		return false;
	}

	ID3D12Device* device = graphicsDevice->GetDevice();

	if (!CreateCommand(device)) {
		Finalize();
		return false;
	}

	if (!CreateFence(device)) {
		Finalize();
		return false;
	}

	LOG_INFO("CommandContext の初期化が正常に完了しました。");

	return true;
}

bool CommandContext::Reset() {
	//アロケータのリセット
	if (FAILED(commandAllocator_->Reset())) {
		LOG_ERROR("CommandAllocator のリセットに失敗しました。");
		return false;
	}

	//コマンドリストのリセット
	if (FAILED(commandList_->Reset(commandAllocator_.Get(), nullptr))) {
		LOG_ERROR("CommandList のリセットに失敗しました。");
		return false;
	}

	return true;
}

bool CommandContext::Execute() {
	if (FAILED(commandList_->Close())) {
		LOG_ERROR("CommandList を Close することに失敗しました(不正なコマンドが記録されている可能性があります)。");
		return false;
	}

	//キューヘ実行を要求
	std::array<ID3D12CommandList*, 1> commandLists = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());

	return true;
}

void CommandContext::WaitForGPU() {
	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	if (FAILED(commandQueue_->Signal(fence_.Get(), fenceValue_))) {
		LOG_ERROR("CommandQueue への Signal 発行に失敗しました。");
		return;
	}

	//Fenceの値が指定したSignal値にたどり着いているかを確認する
	if (fence_->GetCompletedValue() < fenceValue_) {
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		if (SUCCEEDED(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_))) {
			//イベントを待つ
			WaitForSingleObject(fenceEvent_, INFINITE);
		} else {
			LOG_ERROR("SetEventOnCompletion の設定に失敗しました。");
		}
	}
}

void CommandContext::Finalize() {
	if (fenceEvent_ != nullptr) {
		CloseHandle(fenceEvent_);
		fenceEvent_ = nullptr;
	}

	fence_.Reset();
	commandList_.Reset();
	commandAllocator_.Reset();
	commandQueue_.Reset();
}

bool CommandContext::CreateCommand(ID3D12Device* device) {
	//コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;          //登録可能なコマンドリストのタイプ
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; //コマンドキューの優先度
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;          //特性を設定するフラグ
	commandQueueDesc.NodeMask = 0;                                   //GPUが一つであれば0

	//コマンドキュー
	if (FAILED(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_)))) {
		LOG_ERROR("CommandQueue の生成に失敗しました。");
		return false;
	}

	//コマンドアロケータ
	if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_)))) {
		LOG_ERROR("CommandAllocator の生成に失敗しました。");
		return false;
	}

	//コマンドリスト
	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_)))) {
		LOG_ERROR("CommandList の生成に失敗しました。");
		return false;
	}

	//初期化時は一度 Close 状態にしておき、利用時に Reset() から始まる運用にする
	commandList_->Close();

	return true;
}

bool CommandContext::CreateFence(ID3D12Device* device) {
	if (FAILED(device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)))) {
		LOG_ERROR("Fence の生成に失敗しました。");
		return false;
	}

	//FenceのSignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent_ == nullptr) {
		LOG_ERROR("Fence 用のWin32イベント作成に失敗しました。");
		return false;
	}

	return true;
}
