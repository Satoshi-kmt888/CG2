#pragma once
#include <dxcapi.h>
#include <wrl/client.h>

#include <string>

/**
 * \class ShaderCompiler
 * \brief HLSLシェーダーのコンパイルを管理するクラス
 * \details DXC(DirectX Shader Compiler)を使用して、HLSLファイルをコンパイル済みバイナリに変換
 * * 内部でデバッグ情報の埋め込みや最適化の無効化などの設定を行う
 */
class ShaderCompiler {
public:
	//--- コンストラクタ・デストラクタ ---

	ShaderCompiler() = default;
	~ShaderCompiler() = default;

	//--- 公開関数 ---

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

	/** \brief 初期化処理 */
	void Initialize();

private:
	//--- メンバ変数 ---

	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;                //!< DXCユーティリティ
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;         //!< DXCコンパイラ
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr; //!< インクルードハンドラ
};
