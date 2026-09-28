#pragma once

#include "Input/DigitalState.h"

#include <dinput.h>
#include <wrl/client.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr;

/// <summary>
/// キーボードの入力状態を管理するクラス。
/// </summary>
class Keyboard {
public:
	bool Initialize(IDirectInput8* directInput, HWND hwnd);

	void Update();

private:
	static constexpr size_t kKeyCount = 256;

	ComPtr<IDirectInputDevice8> m_device;
	DigitalState<kKeyCount> m_keys;
};
