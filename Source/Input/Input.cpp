#include "Input/Input.h"

#include "Debugger/Logger.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

bool Input::Initialize(HINSTANCE hInstance, HWND hwnd) {
	HRESULT hr = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr
	);
	if (FAILED(hr)) {
		LOG_ERROR("DirectInputの生成に失敗しました。");
		return false;
	}

	if (!m_keyboard.Initialize(directInput_.Get(), hwnd)) {
		return false;
	}

	//マウスデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouse_, nullptr);
	if (FAILED(hr)) {
		LOG_ERROR("マウスデバイスの生成に失敗しました。");
		return false;
	}

	//入力データ形式のセット
	hr = mouse_->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr)) {
		LOG_ERROR("マウスの入力データ形式をセットできませんでした。");
		return false;
	}

	//排他制御レベルのセット
	hr = mouse_->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
	);
	if (FAILED(hr)) {
		LOG_ERROR("キーボードの排他制御レベルをセットできませんでした。");
		return false;
	}

	LOG_INFO("入力機器の初期化に成功しました。");
	return true;
}

void Input::Update() {
	m_keyboard.Update();

	//マウスの更新
	preMouseState_ = mouseState_;
	mouse_->Acquire();
	mouse_->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState_);
}

bool Input::PushMouse(int button) {
	if (button < 0 || button >= 8) return false;
	return (mouseState_.rgbButtons[button] & 0x80) != 0;
}

bool Input::TriggerMouse(int button) {
	if (button < 0 || button >= 8) return false;
	return ((mouseState_.rgbButtons[button] & 0x80) != 0) && ((preMouseState_.rgbButtons[button] & 0x80) == 0);
}

bool Input::ReleaseMouse(int button) {
	if (button < 0 || button >= 8) return false;
	return ((mouseState_.rgbButtons[button] & 0x80) == 0) && ((preMouseState_.rgbButtons[button] & 0x80) != 0);
}

long Input::GetMouseMoveX() const {
	return mouseState_.lX;
}

long Input::GetMouseMoveY() const {
	return mouseState_.lY;
}

long Input::GetMouseWheel() const {
	return mouseState_.lZ;
}
