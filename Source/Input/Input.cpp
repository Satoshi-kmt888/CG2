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
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
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
}

void Input::Update() {
	preKey_ = key_;

	//キーボード情報の取得開始
	keyboard_->Acquire();

	//全キーの入力状態を取得
	keyboard_->GetDeviceState(static_cast<DWORD>(key_.size()), key_.data());
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
