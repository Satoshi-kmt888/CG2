#include "Input/Input.h"

#include "App/WinApp.h"

#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize() {
	HRESULT hr = DirectInput8Create(
		WinApp::GetInstance()->GetHInstance(),
		DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr
	);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, nullptr);
	assert(SUCCEEDED(hr));

	//入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard); //標準形式
	assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(hr));

	//マウスデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouse_, nullptr);
	assert(SUCCEEDED(hr));

	//入力データ形式のセット
	hr = mouse_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	hr = mouse_->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
	);
	assert(SUCCEEDED(hr));
}

void Input::Update() {
	//キーボードの更新
	preKey_ = key_;
	keyboard_->Acquire();
	keyboard_->GetDeviceState(static_cast<DWORD>(key_.size()), key_.data());

	//マウスの更新
	preMouseState_ = mouseState_;
	mouse_->Acquire();
	mouse_->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState_);
}

bool Input::PushKey(uint8_t key) {
	return (key_[key] & 0x80) != 0;
}

bool Input::UpKey(uint8_t key) {
	return (key_[key] & 0x80) == 0;
}

bool Input::TriggerKey(uint8_t key) {
	return ((key_[key] & 0x80) != 0) && ((preKey_[key] & 0x80) == 0);
}

bool Input::ReleaseKey(uint8_t key) {
	return ((key_[key] & 0x80) == 0) && ((preKey_[key] & 0x80) != 0);
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
