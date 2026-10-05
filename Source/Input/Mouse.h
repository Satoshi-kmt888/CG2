#pragma once

#include "Input/DigitalState.h"

#include <dinput.h>
#include <wrl/client.h>

/// <summary>
/// マウス
/// </summary>
class Mouse {
public:
	//==================================================
	// public methods
	//==================================================

	bool Initialize(IDirectInput8* directInput, HWND hwnd);

	void Update();

	bool Push(uint8_t button) const { return m_buttons.Push(button); }
	bool Trigger(uint8_t button)const { return m_buttons.Trigger(button); }
	bool Release(uint8_t button) const { return m_buttons.Release(button); }

	long GetMoveX() const { return m_state.lX; }
	long GetMoveY() const { return m_state.lY; }
	long GetWheel() const { return m_state.lZ; }

private:
	//==================================================
	// private variables
	//==================================================

	static constexpr size_t kButtonCount = 8;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> m_device;
	DigitalState<kButtonCount> m_buttons;
	DIMOUSESTATE2 m_state{};
};

