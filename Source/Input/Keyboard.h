#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include "Input/DigitalState.h"

#include <cstdint>
#include <dinput.h>
#include <Windows.h>
#include <wrl/client.h>

/// <summary>
/// キーボード
/// </summary>
class Keyboard {
public:
	//==================================================
	// public methods
	//==================================================

	[[nodiscard]] bool Initialize(IDirectInput8* directInput, HWND hwnd);

	void Update();

	bool Push(uint8_t key) const { return m_keys.Push(key); }
	bool Trigger(uint8_t key)const { return m_keys.Trigger(key); }
	bool Release(uint8_t key) const { return m_keys.Release(key); }

private:
	//==================================================
	// private variables
	//==================================================

	static constexpr size_t kKeyCount = 256;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> m_device;
	DigitalState<kKeyCount> m_keys;
};
