#pragma once

#include "Input/DigitalState.h"

#include <XInput.h>

enum class GamepadButton : uint8_t {
	DPadUp = 0,
	DPadDown = 1,
	DPadLeft = 2,
	DPadRight = 3,
	Start = 4,
	Back = 5,
	LeftThumb = 6,
	RightThumb = 7,
	LeftShoulder = 8,
	RightShoulder = 9,
	A = 12,
	B = 13,
	X = 14,
	Y = 15,
};

struct GamepadStick {
	float x = 0.0f;
	float y = 0.0f;
};

/// <summary>
/// ゲームパッド
/// </summary>
class Gamepad {
public:
	//==================================================
	// public methods
	//==================================================

	void Update();

	bool IsConnected() const { return m_connected; }

	bool Push(GamepadButton button) const { return m_buttons.Push(static_cast<size_t>(button)); }
	bool Trigger(GamepadButton button) const { return m_buttons.Trigger(static_cast<size_t>(button)); }
	bool Release(GamepadButton button) const { return m_buttons.Release(static_cast<size_t>(button)); }

	GamepadStick GetLeftStick() const { return m_leftStick; }
	GamepadStick GetRightStick() const { return m_rightStick; }

	float GetLeftTrigger() const { return m_leftTrigger; }
	float GetLeftTrigger() const { return m_rightTrigger; }

	void SetVibration(float left, float right);

private:
	//==================================================
	// private methods
	//==================================================

	void Disconnect();

	//==================================================
	// private variables
	//==================================================

	static constexpr size_t kButtonCount = 16;
	static constexpr uint32_t kReconnectInterval = 60;

	uint32_t m_index = 0;
	bool m_connected = false;
	uint32_t m_reconnectCounter = 0;

	DigitalState<kButtonCount> m_buttons;
	GamepadStick m_leftStick;
	GamepadStick m_rightStick;
	float m_leftTrigger = 0.0f;
	float m_rightTrigger = 0.0f;
};
