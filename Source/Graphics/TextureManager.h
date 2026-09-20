#pragma once

#include "Graphics/DescriptorManager.h"

#include <d3d12.h>
#include <DirectXTex.h>
#include <string>
#include <unordered_map>
#include <wrl/client.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// テクスチャ1枚当たりの管理データ構造体
/// </summary>
struct TextureData {
	ComPtr<ID3D12Resource> resource = nullptr;
	DescriptorHandle descriptorHandle{};
	DirectX::TexMetadata metadata{};
};

/// <summary>
/// テクスチャの読み込み・管理を行うシングルトンクラス
/// </summary>
class TextureManager {
public:
	//--- インスタンス管理 ---

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();

	//コピーガード
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	//--- 公開関数 ---

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	/// <param name="commandList"></param>
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// テクスチャを読み込む
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	const TextureData& Load(const std::string& filePath);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

private:
	//--- インスタンス管理 ---

	TextureManager() = default;
	~TextureManager() = default;

	//--- 内部関数 ---

	/// <summary>
	/// ディスク上の画像ファイルをメモリに読み込む
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	DirectX::ScratchImage ReadFile(const std::string& filePath);

	/// <summary>
	/// 画像データをGPU(VRAM)へ転送する
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="mipImages"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	//--- 内部変数 ---

	ID3D12Device* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//管理用コンテナ
	std::unordered_map<std::string, TextureData> textureDataMap_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResource_;
};
