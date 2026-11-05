#include "ShaderCompiler.h"

#include "App/StringUtility.h"
#include "Debugger/Logger.h"

#include <cassert>

#pragma comment(lib, "dxcompiler.lib")

void ShaderCompiler::Initialize() {
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeしないが、includeに対応するための設定を行っておく
	hr = m_utils->CreateDefaultIncludeHandler(&m_include_handler);
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderCompiler::Compile(const std::wstring& filename, const wchar_t* profile) {
	LOG_INFO("シェーダーコンパイルを開始します。 : {}", StringUtility::ConvertString(filename.c_str()));

	//--- HLSLを読み込む ---

	ComPtr<IDxcBlobEncoding> shader_source = nullptr;
	HRESULT hr = m_utils->LoadFile(filename.c_str(), nullptr, &shader_source);
	if (FAILED(hr)) {
		LOG_ERROR("シェーダーファイルの読み込みに失敗しました。 : {}", StringUtility::ConvertString(filename.c_str()));
		return nullptr;
	}

	//読み込んだファイルの内容を設定する
	DxcBuffer shader_source_buffer{};
	shader_source_buffer.Ptr = shader_source->GetBufferPointer();
	shader_source_buffer.Size = shader_source->GetBufferSize();
	shader_source_buffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知

	//--- コンパイル ---

	LPCWSTR arguments[] = {
		filename.c_str(),
		L"-E", L"main",
		L"-T", profile,
		L"-Zi", L"-Qembed_debug",
		L"-Od",
		L"-Zpr"
	};
	//実際にシェーダーをコンパイルする
	ComPtr<IDxcResult> shader_result = nullptr;
	hr = m_compiler->Compile(
		&shader_source_buffer,
		arguments,
		_countof(arguments),
		m_include_handler.Get(),
		IID_PPV_ARGS(&shader_result)
	);
	//コンパイルエラーではなくdxcが起動できないほどの致命的な状況
	if (FAILED(hr) || !shader_result) {
		LOG_ERROR("DXC compiler failed to execute compilation loop.");
		return nullptr;
	}

	//--- 警告・エラーの検証 ---

	OutputCompileErrors(shader_result.Get());

	//--- バイナリの抽出 ---

	//コンパイル結果から実行のバイナリ部分を取得
	ComPtr<IDxcBlob> shader_blob = nullptr;
	hr = shader_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_blob), nullptr);
	assert(SUCCEEDED(hr));

	LOG_INFO("シェーダーのコンパイルに成功しました。 : {}", StringUtility::ConvertString(filename.c_str()));

	return shader_blob;
}

void ShaderCompiler::OutputCompileErrors(IDxcResult* shader_result) const {
	ComPtr<IDxcBlobUtf8> shader_error = nullptr;

	shader_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shader_error), nullptr);
	if (shader_error && shader_error->GetStringLength() != 0) {
		LOG_ERROR("HLSLでコンパイルエラーが発生しました。 : {}", shader_error->GetStringPointer());
		assert(false);
	}
}
