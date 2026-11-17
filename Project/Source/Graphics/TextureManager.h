#pragma once

#include "Graphics/DescriptorManager.h"

#include <d3d12.h>
#include <DirectXTex.h>
#include <string>
#include <unordered_map>
#include <wrl/client.h>

/// <summary>
/// テクスチャ1枚当たりの管理データ構造体
/// </summary>
struct TextureData {
	DirectX::TexMetadata metadata{};
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	DescriptorHandle descriptor_handle{};
};

/// <summary>
/// テクスチャの読み込み・管理を行うシングルトンクラス
/// </summary>
class TextureManager {
public:
	//=========================================================
	// public methods
	//=========================================================

	static TextureManager* GetInstance();

	//コピーガード
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	void Finalize();

	const TextureData& Load(const std::string& filePath);

private:
	//=========================================================
	// private methods
	//=========================================================

	TextureManager() = default;
	~TextureManager() = default;

	//=========================================================
	// private variables
	//=========================================================

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
	[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	//--- 内部変数 ---

	ID3D12Device* m_device = nullptr;
	ID3D12GraphicsCommandList* m_command_list = nullptr;

	//管理用コンテナ
	std::unordered_map<std::string, TextureData> m_data_map;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediate_resource;
};
