#include "GraphicsDevice.h"

#include "App/StringUtility.h"
#include "Debugger/Logger.h"

#include <array>
#include <d3dcommon.h>
#include <dxgi.h>
#include <Windows.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

GraphicsDevice::~GraphicsDevice() {
	Finalize();
}

bool GraphicsDevice::Initialize() {
#ifdef _DEBUG
	EnableDebugLayer();
#endif

	//各初期化を段階的に実行
	if (!CreateDxgiFactory()) {
		Finalize();
		return false;
	}
	if (!SelectAdapter()) {
		Finalize();
		return false;
	}
	if (!CreateDevice()) {
		Finalize();
		return false;
	}

#ifdef _DEBUG
	ConfigureInfoQueue();
#endif

	m_initialized = true;
	LOG_INFO("GraphicsDevice の初期化が正常に終了しました。");
	return true;
}

void GraphicsDevice::Finalize() {
	m_device.Reset();
	m_adapter.Reset();
	m_dxgiFactory.Reset();

	m_initialized = false;
}

#ifdef _DEBUG
void GraphicsDevice::EnableDebugLayer() const {
	ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE); //GPU側でもチェック

		LOG_INFO("D3D12 デバッグレイヤーおよび GPU-Based Validation を有効化しました。");
	}
}
#endif

bool GraphicsDevice::CreateDxgiFactory() {
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&m_dxgiFactory));
	if (FAILED(hr)) {
		LOG_ERROR("DXGIFactory の生成に失敗しました。");
		return false;
	}

	LOG_INFO("DXGIFactory を生成しました。");

	return true;
}

bool GraphicsDevice::SelectAdapter() {
	//パフォーマンスの高い順にアダプターをチェックする
	for (UINT i = 0; m_dxgiFactory->EnumAdapterByGpuPreference
	(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_adapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		if (FAILED(m_adapter->GetDesc3(&adapterDesc))) {
			continue;
		}

		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力
			LOG_INFO("使用するハードウェアアダプターを選択しました: {}",
				StringUtility::ConvertString(adapterDesc.Description));
			return true;
		}
		//ソフトウェアアダプタの場合は見なかったことにする
		m_adapter = nullptr;
	}

	LOG_ERROR("高パフォーマンスなハードウェアアダプターが見つかりませんでした。");

	return false;
}

bool GraphicsDevice::CreateDevice() {
	//機能レベルとその名称を管理する構造体
	struct FeatureLevelInfo {
		D3D_FEATURE_LEVEL level;
		const char* name;
	};

	//対応させたいDirectX12の機能レベルを定義(高い順)
	std::array<FeatureLevelInfo, 3> featureLevels = {
		{
		{D3D_FEATURE_LEVEL_12_2, "12.2"},
		{D3D_FEATURE_LEVEL_12_1, "12.1"},
		{D3D_FEATURE_LEVEL_12_0, "12.0"},
		}
	};

	//高い順に生成できるか試していく
	for (const auto& info : featureLevels) {
		//採用したアダプターでデバイスを生成
		HRESULT hr = D3D12CreateDevice(m_adapter.Get(), info.level, IID_PPV_ARGS(&m_device));
		if (SUCCEEDED(hr)) {
			LOG_INFO("D3D12Device を生成しました。機能レベル (Feature Level): {}", info.name);
			return true;
		}
	}

	LOG_ERROR("D3D12Device の生成に失敗しました（対応する機能レベルがありません）。");

	return false;
}

#ifdef _DEBUG
void GraphicsDevice::ConfigureInfoQueue() const {
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(m_device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//重大なエラー / 通常のエラー / 警告 時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージのID
		std::array<D3D12_MESSAGE_ID, 1> denyIds = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用によるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑止するレベル
		std::array<D3D12_MESSAGE_SEVERITY, 1> severities = { D3D12_MESSAGE_SEVERITY_INFO };

		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = static_cast<UINT>(denyIds.size());
		filter.DenyList.pIDList = denyIds.data();
		filter.DenyList.NumSeverities = static_cast<UINT>(severities.size());
		filter.DenyList.pSeverityList = severities.data();

		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		LOG_INFO("D3D12 InfoQueue デバッグフィルターを設定しました。");
	}
}
#endif
