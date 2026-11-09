#pragma once

#include <d3d12.h>
#include <deque>
#include <vector>
#include <wrl/client.h>

/// <summary>
/// ルートシグネチャを簡易的に構築するコンポーネントクラス
/// </summary>
class RootSignature {
public:
	//=========================================================
	// publice methods
	//=========================================================

	RootSignature() = default;
	~RootSignature() = default;

	/// <summary>
	/// ルートパラメータにCBVデータを追加
	/// </summary>
	/// <param name="shaderRegister">HLSL側のレジスタ番号</param>
	/// <param name="visibility">このパラメータにアクセスできるシェーダーステージ</param>
	void AddCBV(UINT shader_register, D3D12_SHADER_VISIBILITY visibility);

	/// <summary>
	/// ディスクリプターテーブルを追加
	/// </summary>
	/// <param name="ranges">ディスクリプタレンジ</param>
	/// <param name="visibility">このパラメータにアクセスできるシェーダーステージ</param>
	void AddDescriptorTable(const std::vector<D3D12_DESCRIPTOR_RANGE>& ranges,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_PIXEL);

	/// <summary>
	/// 静的サンプラーを追加
	/// </summary>
	/// <param name="shaderRegister">HLSL側のレジスタ番号</param>
	/// <param name="filter">フィルター</param>
	/// <param name="addressMode">アドレスモード</param>
	/// <param name="visibility">このパラメータにアクセスできるシェーダーステージ</param>
	void AddStaticSampler(UINT shader_register, D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE address_mode = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_PIXEL);

	/// <summary>
	/// 生成したパラメータをもとにルートシグネチャを構成
	/// </summary>
	/// <param name="device"></param>
	/// <returns>生成成功時にtrue</returns>
	[[nodiscard]] bool Build(ID3D12Device* device);

	//=========================================================
	// accessor
	//=========================================================

	ID3D12RootSignature* Get() const { return m_root_signature.Get(); }

private:
	//=========================================================
	// private methods
	//=========================================================

	/// <summary>
	/// 設定データ構築用コンテナをクリア
	/// </summary>
	void Clear();

	//=========================================================
	// private variables
	//=========================================================

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root_signature = nullptr;

	//設定データを保持するコンテナ
	std::deque<D3D12_DESCRIPTOR_RANGE> m_descriptor_ranges;
	std::vector<D3D12_ROOT_PARAMETER> m_root_parameters;
	std::vector<D3D12_STATIC_SAMPLER_DESC> m_sampler_descs;
};
