#pragma once
#include <cstdint>

#include <Windows.h>
#include <wrl/client.h>
#include <xaudio2.h>

//サウンドを管理するクラス
class AudioManager {
public:
	//--- 内部データ構造体 ---

	struct ChunkHeader {
		char id[4];   //チャンクごとのID
		int32_t size; //チャンクサイズ
	};

	struct RiffHeader {
		ChunkHeader chunk; //RIFF
		char type[4];       //WAVE
	};

	struct FormatChunk {
		ChunkHeader chunk; //fmt
		WAVEFORMATEX fmt;  //波形フォーマット
	};

	struct SoundData {
		WAVEFORMATEX wfex; //波形フォーマット
		BYTE* pBuffer; //バッファの先頭アドレス
		unsigned int bufferSize; //バッファのサイズ
	};

	//--- インスタンス管理 ---

	//インスタンスの取得
	static AudioManager* GetInstance();

	//コピーガード
	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;

	//--- 公開関数 ---

	//初期化
	void Initialize();

	//読み込み
	SoundData SoundLoadWave(const char* filename);

	//解放
	void SoundUnload(SoundData* soundData);

	//再生
	void SoundPlayWave(const SoundData& soundData);

	//終了処理
	void Finalize();

private:
	//--- コンストラクタ・デストラクタ ---

	AudioManager() = default;
	~AudioManager() = default;

	//--- 内部変数 ---

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
};

