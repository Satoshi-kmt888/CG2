#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include "Input/DigitalState.h"

#include <cstdint>
#include <dinput.h>
#include <Windows.h>
#include <wrl/client.h>

/// <summary>
/// マウスボタンの種類。
/// 値はDIMOUSESTATE2::rgbButtonsの添字と対応しているので、順序を変えないこと
/// </summary>
enum class MouseButton : uint8_t {
	Left,
	Right,
	Middle,
	X1,
	X2,
};

/// <summary>
/// マウス
/// </summary>
class Mouse {
public:
	//==================================================
	// public methods
	//==================================================

	[[nodiscard]] bool Initialize(IDirectInput8* directInput, HWND hwnd);

	void Update();

	bool Push(MouseButton button) const { return m_buttons.Push(static_cast<size_t>(button)); }
	bool Trigger(MouseButton button)const { return m_buttons.Trigger(static_cast<size_t>(button)); }
	bool Release(MouseButton button) const { return m_buttons.Release(static_cast<size_t>(button)); }

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

