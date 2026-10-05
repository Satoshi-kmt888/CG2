#pragma once

#include "Input/DigitalState.h"

#include <cstdint>

/// <summary>
/// ゲームパッドのボタンの種類。
/// 10, 11はXInput側が欠番なので、A以降の値を連番にしないこと
/// </summary>
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

/// <summary>
/// スティックの傾き。デッドゾーン処理済みで、長さは 0.0〜1.0。
/// y は上方向がプラス(画面座標の向きとは逆)
/// </summary>
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

	explicit Gamepad(uint32_t index = 0) :m_index(index) {}

	void Update();

	bool IsConnected() const { return m_connected; }

	bool Push(GamepadButton button) const { return m_buttons.Push(static_cast<size_t>(button)); }
	bool Trigger(GamepadButton button) const { return m_buttons.Trigger(static_cast<size_t>(button)); }
	bool Release(GamepadButton button) const { return m_buttons.Release(static_cast<size_t>(button)); }

	GamepadStick GetLeftStick() const { return m_leftStick; }
	GamepadStick GetRightStick() const { return m_rightStick; }

	float GetLeftTrigger() const { return m_leftTrigger; }
	float GetRightTrigger() const { return m_rightTrigger; }

	/// <summary>
	/// 振動の強さを0.0f~1.0fで設定する
	/// </summary>
	/// <param name="left">低周波モーター</param>
	/// <param name="right">高周波モーター</param>
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
