#include "Gamepad.h"

#include <algorithm>
#include <bitset>
#include <cmath>

#pragma comment (lib, "xinput.lib")

static_assert((1 << static_cast<int>(GamepadButton::DPadUp)) == XINPUT_GAMEPAD_DPAD_UP);
static_assert((1 << static_cast<int>(GamepadButton::Start)) == XINPUT_GAMEPAD_START);
static_assert((1 << static_cast<int>(GamepadButton::LeftShoulder)) == XINPUT_GAMEPAD_LEFT_SHOULDER);
static_assert((1 << static_cast<int>(GamepadButton::A)) == XINPUT_GAMEPAD_A);
static_assert((1 << static_cast<int>(GamepadButton::Y)) == XINPUT_GAMEPAD_Y);

namespace {
	constexpr float kStickMax = 32767.0f;

	GamepadStick ApplyStickDeadzone(SHORT rawX, SHORT rawY, SHORT deadzone) {
		const auto x = static_cast<float>(rawX);
		const auto y = static_cast<float>(rawY);
		const auto magnitude = std::sqrt(x * x + y * y);

		if (magnitude <= static_cast<float>(deadzone)) {
			return{};
		}

		const float clamped = (std::min)(magnitude, kStickMax);
		const float scaled = (clamped - deadzone) / (kStickMax - deadzone);

		return { x / magnitude * scaled, y / magnitude * scaled };
	}

	float ApplyTriggerThreshold(BYTE raw) {
		if (raw <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
			return 0.0f;
		}

		return static_cast<float>(raw - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) /
			static_cast<float>(255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}
}

void Gamepad::Update() {
	if (!m_connected) {
		if (m_reconnectCounter > 0) {
			--m_reconnectCounter;
			return;
		}
		m_reconnectCounter = kReconnectInterval;
	}

	XINPUT_STATE state{};
	if (XInputGetState(m_index, &state) != ERROR_SUCCESS) {
		Disconnect();
		return;
	}

	m_connected = true;
	const XINPUT_GAMEPAD& pad = state.Gamepad;

	m_buttons.Update(std::bitset<kButtonCount>(pad.wButtons));
	m_leftStick = ApplyStickDeadzone(pad.sThumbLX, pad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	m_rightStick = ApplyStickDeadzone(pad.sThumbRX, pad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	m_leftTrigger = ApplyTriggerThreshold(pad.bLeftTrigger);
	m_rightTrigger = ApplyTriggerThreshold(pad.bRightTrigger);
}

void Gamepad::SetVibration(float left, float right) {
	if (!m_connected) {
		return;
	}

	auto toMotor = [](float v) {
		return static_cast<WORD>(std::clamp(v, 0.0f, 1.0f) * 65535.0f);
		};

	XINPUT_VIBRATION vibration{};
	vibration.wLeftMotorSpeed = toMotor(left);
	vibration.wRightMotorSpeed = toMotor(right);
	XInputSetState(m_index, &vibration);
}

void Gamepad::Disconnect() {
	m_connected = false;
	m_buttons.Reset();
	m_leftStick = {};
	m_rightStick = {};
	m_leftTrigger = 0.0f;
	m_rightTrigger = 0.0f;
}
