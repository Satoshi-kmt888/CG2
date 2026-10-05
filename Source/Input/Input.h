#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include "Input/Gamepad.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

#include <dinput.h>
#include <wrl/client.h>

/// <summary>
/// 入力処理を管理するクラス
/// </summary>
class Input {
public:
	//==================================================
	// public methods
	//==================================================

	static Input* GetInstance();

	//コピー・ムーブ禁止
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator=(const Input&&) = delete;

	bool Initialize(HINSTANCE hInstance, HWND hwnd);

	void Update();

	void Finalize();

	bool PushKey(uint8_t key) const { return m_keyboard.Push(key); }
	bool TriggerKey(uint8_t key) const { return m_keyboard.Trigger(key); }
	bool ReleaseKey(uint8_t key) const { return m_keyboard.Release(key); }

	bool PushMouse(uint8_t button) const { return m_mouse.Push(button); }
	bool TriggerMouse(uint8_t button)const { return m_mouse.Trigger(button); }
	bool ReleaseMouse(uint8_t button)const { return m_mouse.Release(button); }

	long GetMouseMoveX() const { return m_mouse.GetMoveX(); }
	long GetMouseMoveY() const { return m_mouse.GetMoveY(); }
	long GetMouseWheel() const { return m_mouse.GetWheel(); }

	bool PushGamepad(GamepadButton button) const { return m_gamepad.Push(button); }
	void SetVibration(float left, float right) { return m_gamepad.SetVibration(left, right); }

private:
	//==================================================
	// private methods
	//==================================================

	Input() = default;
	~Input() = default;

	//==================================================
	// private variables
	//==================================================

	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;

	Keyboard m_keyboard;
	Mouse m_mouse;
	Gamepad m_gamepad{ 0 };
};
