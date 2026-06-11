#pragma once
#include <DirectXMath.h>

#include <cassert>
#include <cmath>

/// @brief 3次元ベクトル
struct alignas(16) Vector3 : public DirectX::XMFLOAT4 {
	Vector3() = default;
	constexpr Vector3(float inX, float inY, float inZ, float inW = 0.0f) noexcept
		:DirectX::XMFLOAT4(inX, inY, inZ, inW) {
	}

	//XMVECTORからの読み込み
	explicit Vector3(DirectX::FXMVECTOR v) noexcept {
		DirectX::XMStoreFloat4(static_cast<DirectX::XMFLOAT4*>(this), v);
	}

	//XMVECTORへの変換
	explicit operator DirectX::XMVECTOR() const noexcept {
		return DirectX::XMLoadFloat4(static_cast<const DirectX::XMFLOAT4*>(this));
	}

	/*--- 複合代入演算子 ---*/

	Vector3& __vectorcall operator+=(const Vector3& vector) noexcept {
		x += vector.x;
		y += vector.y;
		z += vector.z;
		return *this;
	}

	Vector3& __vectorcall operator-=(const Vector3& vector) noexcept {
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		return *this;
	}

	Vector3& __vectorcall operator*=(float scalar) noexcept {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	Vector3& __vectorcall operator/=(float scalar) noexcept {
		assert(scalar != 0.0f);
		float inverse = 1.0f / scalar;

		x *= inverse;
		y *= inverse;
		z *= inverse;

		return *this;
	}

	/*--- 二項演算子 ---*/

	friend Vector3 __vectorcall operator+(Vector3 v1, const Vector3& v2) noexcept {
		v1 += v2;
		return v1;
	}

	friend Vector3 __vectorcall operator-(Vector3 v1, const Vector3& v2) noexcept {
		v1 -= v2;
		return v1;
	}

	friend Vector3 __vectorcall operator*(Vector3 v, float scalar) noexcept {
		v *= scalar;
		return v;
	}

	friend Vector3 __vectorcall operator*(float scalar, const Vector3& v) noexcept {
		return v * scalar;
	}

	/*--- 比較演算 ---*/

	bool __vectorcall operator==(const Vector3& vector) const noexcept = default;

	/*--- 基本操作 ---*/

	//ベクトルの長さを求める
	float Length() const noexcept {
		return std::sqrtf((x * x) + (y * y) + (z * z));
	}

	//正規化
	Vector3 Normalized() const noexcept {
		if (float lengthSquared = (x * x) + (y * y) + (z * z); lengthSquared > 0.0f) {
			float inverseLength = 1.0f / std::sqrtf(lengthSquared);
			return { x * inverseLength, y * inverseLength, z * inverseLength };
		}

		return { 0.0f, 0.0f, 0.0f };
	}
};

/*--- ベクトル演算 ---*/

//正規化
inline Vector3 __vectorcall Normalize(const Vector3& vector) noexcept {
	return vector.Normalized();
}

//内積
inline float __vectorcall Dot(const Vector3& v1, const Vector3& v2) noexcept {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

//外積
inline Vector3 __vectorcall Cross(const Vector3& v1, const Vector3& v2) noexcept {
	return Vector3{
		(v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x)
	};
}
