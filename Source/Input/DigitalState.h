#pragma once

#include <array>
#include <cstddef>

/// <summary>
/// N個のボタンを現在フレームと1フレーム前の状態でフラグを管理するクラス。
/// </summary>
/// <typeparam name="N">ボタン数</typeparam>
template<size_t N>
class DigitalState {
public:
	void Update(const uint8_t* raw) {
		m_pre = m_cur;
		for (size_t i = 0; i < N; ++i) {
			m_cur = (raw[i] & std::byte{ 0x80 }) != std::byte{ 0 };
		}
	}

	void Reset() {
		m_pre = m_cur;
		m_cur.fill(false);
	}

	bool Push(size_t index) { return InRange(index) && m_cur[index]; }
	bool Trigger(size_t index) { return InRange(index) && m_cur[index] && !m_pre[index]; };
	bool Release(size_t index) { return InRange(index) && !m_cur[index] && m_pre[index]; };

private:
	static constexpr bool InRange(size_t index) { return index < N; }

	std::array<bool, N> m_cur{};
	std::array<bool, N> m_pre{};
};
