#pragma once
#include <d3d12sdklayers.h>
#include <dxgi1_3.h>
#include <dxgidebug.h>
#include <wrl/client.h>

/**
 * \class D3D12ResourceLeakChecker
 * \brief リークチェックを管理するクラス
 */
class D3D12ResourceLeakChecker {
public:
	D3D12ResourceLeakChecker() = default;

	//デストラクでリークチェック
	~D3D12ResourceLeakChecker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}

	//コピーガード
	D3D12ResourceLeakChecker(const D3D12ResourceLeakChecker&) = delete;
	D3D12ResourceLeakChecker& operator=(const D3D12ResourceLeakChecker&) = delete;
};

