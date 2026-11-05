#pragma once

#include <Windows.h>

#include <dxcapi.h>
#include <string>
#include <wrl/client.h>

/// <summary>
/// HLSLシェーダーのコンパイル
/// </summary>
class ShaderCompiler {
public:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//============================================================
	// public methods
	//============================================================

	ShaderCompiler() = default;
	~ShaderCompiler() = default;

	void Initialize();

	/// <summary>
	/// HLSLファイルを読み込み、コンパイルしてバイナリを取得する
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="profile"></param>
	/// <returns></returns>
	ComPtr<IDxcBlob> Compile(const std::wstring& filename, const wchar_t* profile);

private:
	//============================================================
	// private methods
	//============================================================

	/// <summary>
	/// コンパイルエラーを解析し、詳細なログを出力
	/// </summary>
	/// <param name="shdaerResult"></param>
	void OutputCompileErrors(IDxcResult* shader_result) const;

	//============================================================
	// private variables
	//============================================================

	ComPtr<IDxcUtils> m_utils = nullptr;
	ComPtr<IDxcCompiler3> m_compiler = nullptr;
	ComPtr<IDxcIncludeHandler> m_include_handler = nullptr;
};
