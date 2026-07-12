#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <array>
#include <dinput.h>

//入力処理を管理するクラス
class Input {
public:
	//--- インスタンス管理 ---

	//インスタンスの取得
	static Input* GetInstance();

	//コピーガード
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	//---公開関数 ---

	void Initialize();

	void Update();

	bool PushKey(uint8_t key); //押してるとき
	bool UpKey(uint8_t key); //離してるとき
	bool TriggerKey(uint8_t key); //押した瞬間
	bool ReleaseKey(uint8_t key); //離した瞬間

private:
	//--- コンストラクタ・デストラクタ ---

	Input() = default;
	~Input() = default;

	//--- 内部変数 ---

	IDirectInput8* directInput_ = nullptr;
	IDirectInputDevice8* keyboard_ = nullptr;
	std::array<BYTE, 256> key_{};
	std::array<BYTE, 256> preKey_{};
};
