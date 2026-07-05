#include "InputManager.h"

#include "FrameWork/WinApp.h"

#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

InputManager* InputManager::GetInstance() {
	static InputManager instance;
	return &instance;
}

void InputManager::Initialize() {
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

void InputManager::Update() {
	preKey_ = key_;

	//キーボード情報の取得開始
	keyboard_->Acquire();

	//全キーの入力状態を取得
	keyboard_->GetDeviceState(static_cast<DWORD>(key_.size()), key_.data());
}

bool InputManager::PushKey(uint8_t key) {
	return (key_[key] & 0x80) != 0;
}

bool InputManager::UpKey(uint8_t key) {
	return (key_[key] & 0x80) == 0;
}

bool InputManager::TriggerKey(uint8_t key) {
	return ((key_[key] & 0x80) != 0) && ((preKey_[key] & 0x80) == 0);
}

bool InputManager::ReleaseKey(uint8_t key) {
	return ((key_[key] & 0x80) == 0) && ((preKey_[key] & 0x80) != 0);
}
