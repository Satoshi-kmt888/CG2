#include "Input/Keyboard.h"

#include "Debugger/Logger.h"

#include <array>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

bool Keyboard::Initialize(IDirectInput8* directInput, HWND hwnd) {
	if (!directInput) {
		LOG_ERROR("引数がnullptrです。DirectInputの生成に失敗している可能性があります。");
		return false;
	}

	if (FAILED(directInput->CreateDevice(GUID_SysKeyboard, &m_device, nullptr))) {
		LOG_ERROR("キーボードデバイスの生成に失敗しました。");
		return false;
	}

	if (FAILED(m_device->SetDataFormat(&c_dfDIKeyboard))) {
		LOG_ERROR("キーボードの入力データ形式をセットできませんでした。");
		return false;
	}

	if (FAILED(m_device->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY))) {
		LOG_ERROR("キーボードの排他制御レベルをセットできませんでした。");
		return false;
	}

	LOG_INFO("キーボードデバイスの初期化に成功しました。");
	return true;
}

void Keyboard::Update() {
	if (!m_device) {
		return;
	}

	std::array<uint8_t, kKeyCount> raw{};
	HRESULT hr = m_device->GetDeviceState(static_cast<DWORD>(sizeof(raw)), raw.data());

	if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED && SUCCEEDED(m_device->Acquire())) {
		hr = m_device->GetDeviceState(static_cast<DWORD>(sizeof(raw)), raw.data());
	}

	if (FAILED(hr)) {
		m_keys.Reset();
		return;
	}

	m_keys.Update(raw);
}
