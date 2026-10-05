#include "Mouse.h"

#include "Debugger/Logger.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

bool Mouse::Initialize(IDirectInput8* directInput, HWND hwnd) {
	if (!directInput) {
		LOG_ERROR("引数がnullptrです。DirectInputの生成に失敗している可能性があります。");
		return false;
	}

	if (FAILED(directInput->CreateDevice(GUID_SysMouse, &m_device, nullptr))) {
		LOG_ERROR("マウスデバイスの生成に失敗しました。");
		return false;
	}

	if (FAILED(m_device->SetDataFormat(&c_dfDIMouse2))) {
		LOG_ERROR("マウスの入力データ形式をセットできませんでした。");
		m_device.Reset();
		return false;
	}

	//FOREGROUND: ウィンドウが非アクティブの間は入力を受け取らない
	//NONEXCLUSIVE: 他のアプリのマウス操作を妨げない(カーソルも表示されたまま)
	if (FAILED(m_device->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
		LOG_ERROR("マウスの排他制御レベルをセットできませんでした。");
		m_device.Reset();
		return false;
	}

	LOG_INFO("マウスデバイスの初期化に成功しました。");
	return true;
}

void Mouse::Update() {
	if (!m_device) {
		return;
	}

	DIMOUSESTATE2 state{};
	HRESULT hr = m_device->GetDeviceState(sizeof(state), &state);

	if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
		if (SUCCEEDED(m_device->Acquire())) {
			hr = m_device->GetDeviceState(sizeof(state), &state);
		}
	}

	if (FAILED(hr)) {
		m_buttons.Reset();
		m_state = {};
		return;
	}

	m_state = state;
	m_buttons.Update(state.rgbButtons);
}
