#include "Gamepad.h"

#include <algorithm>
#include <bitset>
#include <cmath>
#include <Windows.h>
#include <Xinput.h>

#pragma comment (lib, "xinput.lib")

namespace {
	constexpr int Bit(GamepadButton button) { return 1 << static_cast<int>(button); }

	// enumの値がXInputの定義とずれていないことをコンパイル時に確認する
	static_assert(Bit(GamepadButton::DPadUp) == XINPUT_GAMEPAD_DPAD_UP);
	static_assert(Bit(GamepadButton::DPadDown) == XINPUT_GAMEPAD_DPAD_DOWN);
	static_assert(Bit(GamepadButton::DPadLeft) == XINPUT_GAMEPAD_DPAD_LEFT);
	static_assert(Bit(GamepadButton::DPadRight) == XINPUT_GAMEPAD_DPAD_RIGHT);
	static_assert(Bit(GamepadButton::Start) == XINPUT_GAMEPAD_START);
	static_assert(Bit(GamepadButton::Back) == XINPUT_GAMEPAD_BACK);
	static_assert(Bit(GamepadButton::LeftThumb) == XINPUT_GAMEPAD_LEFT_THUMB);
	static_assert(Bit(GamepadButton::RightThumb) == XINPUT_GAMEPAD_RIGHT_THUMB);
	static_assert(Bit(GamepadButton::LeftShoulder) == XINPUT_GAMEPAD_LEFT_SHOULDER);
	static_assert(Bit(GamepadButton::RightShoulder) == XINPUT_GAMEPAD_RIGHT_SHOULDER);
	static_assert(Bit(GamepadButton::A) == XINPUT_GAMEPAD_A);
	static_assert(Bit(GamepadButton::B) == XINPUT_GAMEPAD_B);
	static_assert(Bit(GamepadButton::X) == XINPUT_GAMEPAD_X);
	static_assert(Bit(GamepadButton::Y) == XINPUT_GAMEPAD_Y);

	constexpr float kStickMax = 32767.0f;

	GamepadStick ApplyStickDeadzone(SHORT raw_x, SHORT raw_y, SHORT deadzone) {
		const auto x = static_cast<float>(raw_x);
		const auto y = static_cast<float>(raw_y);
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
		if (m_reconnect_counter > 0) {
			--m_reconnect_counter;
			return;
		}
		m_reconnect_counter = k_reconnect_interval;
	}

	XINPUT_STATE state{};
	if (XInputGetState(m_index, &state) != ERROR_SUCCESS) {
		Disconnect();
		return;
	}

	m_connected = true;
	const XINPUT_GAMEPAD& pad = state.Gamepad;

	m_buttons.Update(std::bitset<k_button_count>(pad.wButtons));
	m_left_stick = ApplyStickDeadzone(pad.sThumbLX, pad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	m_right_stick = ApplyStickDeadzone(pad.sThumbRX, pad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	m_left_trigger = ApplyTriggerThreshold(pad.bLeftTrigger);
	m_right_trigger = ApplyTriggerThreshold(pad.bRightTrigger);
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
	m_left_stick = {};
	m_right_stick = {};
	m_left_trigger = 0.0f;
	m_right_trigger = 0.0f;
}
