#include "Input/Input.h"

#include "App/WinApp.h"
#include "Debugger/Logger.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

bool Input::Initialize(WinApp* win_app) {
	HRESULT hr = DirectInput8Create(
		win_app->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&m_direct_input, nullptr
	);
	if (FAILED(hr)) {
		LOG_ERROR("DirectInputの生成に失敗しました。");
		return false;
	}

	if (!m_keyboard.Initialize(m_direct_input.Get(), win_app->GetHwnd())) {
		return false;
	}

	if (!m_mouse.Initialize(m_direct_input.Get(), win_app->GetHwnd())) {
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
	m_gamepad.SetVibration(0.0f, 0.0f);
}
