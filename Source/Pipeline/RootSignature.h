#pragma once

#include <d3d12.h>
#include <deque>
#include <vector>
#include <wrl/client.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// ルートシグネチャ簡易的に構築するコンポーネントクラス
/// </summary>
class RootSignature {
public:
	//--- インスタンス管理 ---

	RootSignature() = default;
	~RootSignature() = default;

	//--- 公開関数 ---

	/// <summary>
	/// ルートパラメータにCBVデータを追加
	/// </summary>
	/// <param name="shaderRegister">HLSL側のレジスタ番号</param>
	/// <param name="visibility">このパラメータにアクセスできるシェーダーステージ</param>
	void AddCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility);

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
	void AddStaticSampler(UINT shaderRegister, D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE addressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_PIXEL);

	/// <summary>
	/// 生成したパラメータをもとにルートシグネチャを構成
	/// </summary>
	/// <param name="device"></param>
	/// <returns>生成成功時にtrue</returns>
	[[nodiscard]] bool Build(ID3D12Device* device);

	//--- ゲッター ---

	ID3D12RootSignature* Get() const { return rootSignature_.Get(); }

private:
	//--- 内部関数 ---

	/// <summary>
	/// 設定データ構築用コンテナをクリア
	/// </summary>
	void Clear();

	//--- 内部変数 ---

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	//設定データを保持するコンテナ
	std::deque<D3D12_DESCRIPTOR_RANGE> descriptorRanges_;
	std::vector<D3D12_ROOT_PARAMETER> rootParameters_;
	std::vector<D3D12_STATIC_SAMPLER_DESC> samplerDescs_;
};
