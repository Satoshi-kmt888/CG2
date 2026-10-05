#pragma once

#include <bitset>
#include <cstdint>
#include <span>

/// <summary>
/// N個のボタンを現在フレームと1フレーム前の状態でフラグを管理するクラス
/// </summary>
/// <typeparam name="N">ボタン数</typeparam>
template<size_t N>
class DigitalState {
public:
	//==================================================
	// public methods
	//==================================================

	void Update(std::span<const uint8_t, N> raw) {
		m_pre = m_cur;
		for (size_t i = 0; i < N; ++i) {
			m_cur[i] = (raw[i] & kPressedMask) != 0;
		}
	}

	void Reset() {
		m_pre = m_cur;
		m_cur.reset();
	}

	bool Push(size_t index) const { return InRange(index) && m_cur[index]; }
	bool Trigger(size_t index) const { return InRange(index) && m_cur[index] && !m_pre[index]; }
	bool Release(size_t index) const { return InRange(index) && !m_cur[index] && m_pre[index]; }

private:
	//==================================================
	// private methods
	//==================================================

	static constexpr bool InRange(size_t index) { return index < N; }

	//==================================================
	// private variables
	//==================================================

	static constexpr uint8_t kPressedMask = 0x80;

	std::bitset<N> m_cur{};
	std::bitset<N> m_pre{};
};
