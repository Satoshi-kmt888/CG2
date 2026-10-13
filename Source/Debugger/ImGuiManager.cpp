#include "ImGuiManager.h"

#ifdef USE_IMGUI
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(HWND hwnd, ID3D12Device* device, int bufferCount, DXGI_FORMAT rtvFormat) {
#ifdef USE_IMGUI
	m_hwnd = hwnd;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);

	auto descriptorManager = DescriptorManager::GetInstance();
	ID3D12DescriptorHeap* descriptorHeap = descriptorManager->GetHeap(DescriptorType::SRV_CBV_UAV);
	descriptorHandle_ = descriptorManager->Allocate(DescriptorType::SRV_CBV_UAV);
	ImGui_ImplDX12_Init(
		device, bufferCount, rtvFormat, descriptorHeap,
		descriptorHandle_.cpuHandle,
		descriptorHandle_.gpuHandle
	);
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Build();
#endif
}

void ImGuiManager::BeginFrame() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}

void ImGuiManager::EndFrame(ID3D12GraphicsCommandList* commandList) {
#ifdef USE_IMGUI
	//ImGUiの描画コマンドを確定させる
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif
}

void ImGuiManager::Finalize() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DescriptorManager::GetInstance()->Free(descriptorHandle_);
#endif
}

bool ImGuiManager::HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef USE_IMGUI
	if (hwnd != GetInstance()->m_hwnd) {
		return false;
	}

	return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam) != 0;
#else
	return false;
#endif
}
