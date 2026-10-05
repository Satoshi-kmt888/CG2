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

	if (!m_mouse.Initialize(directInput_.Get(), hwnd)) {
		return false;
	}

	LOG_INFO("入力機器の初期化に成功しました。");
	return true;
}

void Input::Update() {
	m_keyboard.Update();
	m_mouse.Update();
	m_gamepad.Update();
}

void Input::Finalize() {
}
