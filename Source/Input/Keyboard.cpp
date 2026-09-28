#include "Keyboard.h"

#include "Debugger/Logger.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

bool Keyboard::Initialize(IDirectInput8* directInput, HWND hwnd) {
	if (!directInput) {
		LOG_ERROR("引数がnullptrです。DirectInputの生成に失敗している可能性があります。");
		return false;
	}

	//デバイスの生成
	if (FAILED(directInput->CreateDevice(GUID_SysKeyboard, &m_device, nullptr))) {
		LOG_ERROR("キーボードデバイスの生成に失敗しました。");
		return false;
	}

	//入力データ形式のセット
	if (FAILED(m_device->SetDataFormat(&c_dfDIKeyboard))) {
		LOG_ERROR("キーボードの入力データ形式をセットできませんでした。");
		return false;
	}

	//排他制御レベルのセット
	if (FAILED(m_device->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	))) {
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

	uint8_t raw[kKeyCount]{};
	HRESULT hr = m_device->GetDeviceState(static_cast<DWORD>(sizeof(raw)), raw);

	if (FAILED(hr)) {
		m_device->Acquire();
		m_keys.Reset();
		return;
	}

	m_keys.Update(raw);
}
