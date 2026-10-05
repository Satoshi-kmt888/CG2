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
	Input& operator=(Input&&) = delete;

	[[nodiscard]]bool Initialize(HINSTANCE hInstance, HWND hwnd);

	void Update();

	void Finalize();

	const Keyboard& GetKeyboard() const { return m_keyboard; }
	const Mouse& GetMouse() const { return m_mouse; }
	const Gamepad& GetGamepad() const { return m_gamepad; }
	Gamepad& GetGamepad() { return m_gamepad; }

private:
	//==================================================
	// private methods
	//==================================================

	Input() = default;
	~Input() = default;

	//==================================================
	// private variables
	//==================================================

	Microsoft::WRL::ComPtr<IDirectInput8> m_directInput;

	Keyboard m_keyboard;
	Mouse m_mouse;
	Gamepad m_gamepad{ 0 };
};
