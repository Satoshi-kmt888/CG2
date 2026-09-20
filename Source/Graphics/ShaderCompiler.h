#pragma once

#include <Windows.h>
#include <dxcapi.h>
#include <wrl/client.h>
#include <string>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// HLSLシェーダーのコンパイルを管理するクラス
/// </summary>
class ShaderCompiler {
public:
	//--- コンストラクタ・デストラクタ ---

	ShaderCompiler() = default;
	~ShaderCompiler() = default;

	//--- 公開関数 ---

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// HLSLファイルを読み込み、コンパイルしてバイナリを取得する
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="profile"></param>
	/// <returns></returns>
	ComPtr<IDxcBlob> Compile(const std::wstring& filePath, const wchar_t* profile);

private:
	//--- 内部関数 ---

	/// <summary>
	/// コンパイルエラーを解析し、詳細なログを出力
	/// </summary>
	/// <param name="shdaerResult"></param>
	void OutputCompileErrors(IDxcResult* shaderResult);

	//--- 内部変数 ---

	ComPtr<IDxcUtils> dxcUtils_ = nullptr;                //!< DXCユーティリティ
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;         //!< DXCコンパイラ
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr; //!< インクルードハンドラ
};
