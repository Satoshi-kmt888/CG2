#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <DirectXTex.h>

/**
 * \struct TextureData
 * \brief テクスチャ1枚当たりの管理データ構造体
 */
struct TextureData {
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
	DirectX::TexMetadata metadata{};
};

/**
 * \class TextureManager
 * \brief テクスチャの読み込み・管理を行うシングルトンクラス
 */
class TextureManager {
public:
	//--- 公開定数 ---

	static const uint32_t kMaxTextures = 128; //<! 最大テクスチャ数

	//--- インスタンス管理 ---

	/**
	 * \brief シングルトンインスタンスの取得
	 * \return TextureManagerのインスタンスポインタ
	 */
	static TextureManager* GetInstance();

	//コピーガード
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	//--- 公開関数 ---

	/**
	 * \brief テクスチャを読み込む
	 * \details 既に読み込み済みのパスが渡された場合、キャッシュからデータを返す
	 * \param[in] filePath 画像ファイルのパス
	 * \return 読み込んだテクスチャの管理データ
	 */
	const TextureData& Load(const std::string& filePath);

	/**
	 * \brief 初期化処理
	 * \param[in] device 使用するDirectX12デバイス
	 * \param[in] commandList 転送コマンドの記録に使用するコマンドリスト
	 */
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	/** \brief 終了処理 */
	void Finalize();

	//--- ゲッター ---

	ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return srvDescriptorHeap_.Get(); }

private:
	//--- コンストラクタ・デストラクタ ---

	TextureManager() = default;
	~TextureManager() = default;

	//--- 内部関数 ---

	/**
	 * \brief ディスク上の画像ファイルをメモリに読み込む
	 * \param[in] filePath 画像ファイルのパス
	 * \return 読み込まれた画像イメージ
	 */
	DirectX::ScratchImage ReadFile(const std::string& filePath);

	/**
	 * \brief 画像データをGPU(VRAM)へ転送する
	 * \param[in] texture 転送先のリソース
	 * \param[in] mipImages 転送元の画像イメージ
	 * \return アップロードに使用した中間リソース
	 */
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	//--- メンバ変数 ---

	//DirectXCommonから参照するオブジェクト
	ID3D12Device* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//リソース管理
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	uint32_t srvDescriptorSize_ = 0; //<! ディスクリプタ1つ分のサイズ
	uint32_t srvDescriptorIndex_ = 0; //<! 現在使用中のディスクリプタ

	//管理用コンテナ
	std::unordered_map<std::string, TextureData> textureDataMap_; //<! ファイルパスをキーにしたテクスチャデータ
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResource_; //<! 転送完了まで保持が必要な中間リソース
};
