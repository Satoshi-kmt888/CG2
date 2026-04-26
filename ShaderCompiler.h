#pragma once

#include <string>

#include <dxcapi.h>
#include <wrl/client.h>

/**
 * \class ShaderCompiler
 * \brief HLSLシェーダーのコンパイルを管理するクラス
 * \details DXC(DirectX Shader Compiler)を使用して、HLSLファイルをコンパイル済みバイナリに変換
 * * 内部でデバッグ情報の埋め込みや最適化の無効化などの設定を行う
 */
class ShaderCompiler {
public://--- ライフサイクル ---
	/** \brief 初期化処理 */
	void Initialize();

public://--- 主要機能 ---
	/**
	 * \brief HLSLファイルを読み込み、コンパイルしてバイナリを取得する
	 * \param[in] filePath コンパイル対象のシェーダーファイルパス
	 * \param profile シェーダープロファイル
	 * \return コンパイル済みバイナリ。エラー時はアサートで停止。
	 */
	Microsoft::WRL::ComPtr<IDxcBlob> Compile(
		const std::wstring& filePath,
		const wchar_t* profile
	);

private://--- メンバ変数 ---
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;                //!< DXCユーティリティ
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;         //!< DXCコンパイラ
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr; //!< インクルードハンドラ
};
