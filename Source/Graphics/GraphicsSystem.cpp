#include "GraphicsSystem.h"

#include "Core/WinApp.h"
#include "Diagnostics/Logger.h"

#include <d3d12.h>

GraphicsSystem* GraphicsSystem::GetInstance() {
	static GraphicsSystem instance;
	return &instance;
}

bool GraphicsSystem::Initialize(const WinApp& winApp) {
	if (!device_.Initialize()) {
		LOG_ERROR("GraphicsDevice の初期化に失敗しました。");
		return false;
	}

	if (!command_.Initialize(&device_)) {
		LOG_ERROR("CommandContext の初期化に失敗しました。");
		Finalize();
		return false;
	}

	if (!swapChain_.Initialize(device_.GetDxgiFactory(), command_.GetCommandQueue(),
		device_.GetDevice(), winApp.GetHwnd(), winApp.GetClientWidth(), winApp.GetClientHeight())) {
		LOG_ERROR("SwapChain の初期化に失敗しました。");
		Finalize();
		return false;
	}

	LOG_INFO("GraphicsSystem の初期化が正常に完了しました。");

	return true;
}

void GraphicsSystem::PreDraw() {
	//次のフレーム用にコマンドアロケータとリストをクリア
	command_.Reset();

	ID3D12GraphicsCommandList* commandList = command_.GetCommandList();
	swapChain_.TransitionToRenderTarget(commandList);

	//描画先となるRTVとDSVのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain_.GetCurrentRtvHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = swapChain_.GetDsvHandle();

	//レンダーターゲット(色)と深度バッファ(奥行き)をクリア
	commandList->ClearRenderTargetView(rtvHandle, kClearColor.data(), 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//描画先のRTVとDSVを設定する
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
}

void GraphicsSystem::PostDraw() {
	ID3D12GraphicsCommandList* commandList = command_.GetCommandList();

	swapChain_.TransitionToPresent(commandList);

	//コマンドリストを確定させ、GPUのキューに実行をリクエスト
	if (!command_.Execute()) {
		LOG_ERROR("描画コマンドの実行要求に失敗しました。");
		//致命的なエラーが発生しているので終了処理を行い安全に閉じる
		Finalize();
		return;
	}

	//画面をフリップ(表示を切り替え)
	if (!swapChain_.Present()) {
		LOG_ERROR("画面のフリップ（Present）に失敗しました。デバイスロストの可能性があります。");
		return;
	}

	//GPUが現在のフレームの描画を終えるまでCPUを停止して待機
	command_.WaitForGPU();
}

void GraphicsSystem::Finalize() {
	swapChain_.Finalize();
	command_.Finalize();
	device_.Finalize();

	LOG_INFO("GraphicsSystem の解放処理が完了しました。");
}
