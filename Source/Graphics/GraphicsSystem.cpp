#include "GraphicsSystem.h"

#include "Debugger/Logger.h"
#include "DescriptorManager.h"

#include <cstdint>
#include <d3d12.h>
#include <thread>

GraphicsSystem* GraphicsSystem::GetInstance() {
	static GraphicsSystem instance;
	return &instance;
}

bool GraphicsSystem::Initialize(HWND hwnd, uint32_t width, uint32_t height) {
	if (!m_device.Initialize()) {
		LOG_ERROR("GraphicsDevice の初期化に失敗しました。");
		return false;
	}

	if (!m_command.Initialize(m_device.GetDevice())) {
		LOG_ERROR("CommandContext の初期化に失敗しました。");
		Finalize();
		return false;
	}

	//スワップチェーンより前に初期化
	DescriptorManager::GetInstance()->Initialize(m_device.GetDevice());

	if (!m_swap_chain.Initialize(m_device.GetDxgiFactory(), m_command.GetCommandQueue(),
		m_device.GetDevice(), hwnd, width, height)) {
		LOG_ERROR("SwapChain の初期化に失敗しました。");
		Finalize();
		return false;
	}

	m_scissor_rect = D3D12_RECT{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	m_viewport = D3D12_VIEWPORT{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };

	InitializeFixFPS();

	LOG_INFO("GraphicsSystem の初期化が正常に完了しました。");
	return true;
}

void GraphicsSystem::PreDraw() {
	ID3D12GraphicsCommandList* commandList = m_command.GetCommandList();

	ID3D12DescriptorHeap* srvDescriptorHeaps[] = {
		DescriptorManager::GetInstance()->GetHeap(DescriptorType::SRV_CBV_UAV)
	};
	commandList->SetDescriptorHeaps(_countof(srvDescriptorHeaps), srvDescriptorHeaps);

	m_swap_chain.TransitionToRenderTarget(commandList);

	//描画先となるRTVとDSVのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swap_chain.GetCurrentRtvHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_swap_chain.GetDsvHandle();

	//レンダーターゲット(色)と深度バッファ(奥行き)をクリア
	commandList->ClearRenderTargetView(rtvHandle, k_clear_color.data(), 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//描画先のRTVとDSVを設定する
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	commandList->RSSetViewports(1, &m_viewport);
	commandList->RSSetScissorRects(1, &m_scissor_rect);
}

void GraphicsSystem::PostDraw() {
	ID3D12GraphicsCommandList* commandList = m_command.GetCommandList();
	m_swap_chain.TransitionToPresent(commandList);

	//コマンドリストを確定させ、GPUのキューに実行をリクエスト
	if (!m_command.Execute()) {
		LOG_ERROR("描画コマンドの実行要求に失敗しました。");
		//致命的なエラーが発生しているので終了処理を行い安全に閉じる
		Finalize();
		return;
	}

	//画面をフリップ(表示を切り替え)
	if (!m_swap_chain.Present()) {
		LOG_ERROR("画面のフリップ(Present)に失敗しました。デバイスロストの可能性があります。");
		return;
	}

	//GPUが現在のフレームの描画を終えるまでCPUを停止して待機
	m_command.WaitForGPU();

	UpdateFixFPS();

	//次のフレーム用にコマンドアロケータとリストをクリア
	m_command.Reset();
}

void GraphicsSystem::Finalize() {
	m_swap_chain.Finalize();
	m_command.Finalize();
	m_device.Finalize();

	DescriptorManager::GetInstance()->Finalize();

	LOG_INFO("GraphicsSystem の解放処理が完了しました。");
}

void GraphicsSystem::InitializeFixFPS() {
	m_reference = std::chrono::steady_clock::now();
}

void GraphicsSystem::UpdateFixFPS() {
	const std::chrono::microseconds k_min_time(uint64_t(1000000.0f / 60.0f));
	const std::chrono::microseconds k_min_check_time(uint64_t(1000000.0f / 65.0f));

	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - m_reference);

	if (elapsed < k_min_check_time) {
		while (std::chrono::steady_clock::now() - m_reference < k_min_time) {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	m_reference = std::chrono::steady_clock::now();
}
