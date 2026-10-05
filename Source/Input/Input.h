#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include "Input/Keyboard.h"

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
	Input(const Input&&) = delete;
	Input& operator=(const Input&&) = delete;

	bool Initialize(HINSTANCE hInstance, HWND hwnd);

	void Update();

	bool PushKey(uint8_t key) const { return m_keyboard.Push(key); }
	bool TriggerKey(uint8_t key) const { return m_keyboard.Trigger(key); }
	bool ReleaseKey(uint8_t key) const { return m_keyboard.Release(key); }

	bool PushMouse(int button);
	bool TriggerMouse(int button);
	bool ReleaseMouse(int button);

	long GetMouseMoveX() const;
	long GetMouseMoveY() const;
	long GetMouseWheel() const;

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

	IDirectInputDevice8* mouse_ = nullptr;
	DIMOUSESTATE2 mouseState_{};
	DIMOUSESTATE2 preMouseState_{};
};
