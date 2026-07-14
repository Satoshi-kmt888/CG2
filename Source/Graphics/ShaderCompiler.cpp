#include "ShaderCompiler.h"

#include "Debugger/Logger.h"
#include "App/StringUtility.h"

#include <array>
#include <cassert>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")

void ShaderCompiler::Initialize() {
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	//現時点でincludeしないが、includeに対応するための設定を行っておく
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

ComPtr<IDxcBlob> ShaderCompiler::Compile(const std::wstring& filePath, const wchar_t* profile) {
	LOG_INFO("シェーダーコンパイルを開始します。 : {}", StringUtility::ConvertString(filePath.c_str()));

	//--- HLSLを読み込む ---

	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	if (FAILED(hr)) {
		LOG_ERROR("シェーダーファイルの読み込みに失敗しました。 : {}", StringUtility::ConvertString(filePath.c_str()));
		return nullptr;
	}

	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer{};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知

	//--- コンパイル ---

	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E", L"main",
		L"-T", profile,
		L"-Zi", L"-Qembed_debug",
		L"-Od",
		L"-Zpr"
	};
	//実際にシェーダーをコンパイルする
	ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_.Get(),
		IID_PPV_ARGS(&shaderResult)
	);
	//コンパイルエラーではなくdxcが起動できないほどの致命的な状況
	if (FAILED(hr) || !shaderResult) {
		LOG_ERROR("DXC compiler failed to execute compilation loop.");
		return nullptr;
	}

	//--- 警告・エラーの検証 ---

	OutputCompileErrors(shaderResult.Get());

	//--- バイナリの抽出 ---

	//コンパイル結果から実行のバイナリ部分を取得
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	LOG_INFO("シェーダーのコンパイルに成功しました。 : {}", StringUtility::ConvertString(filePath.c_str()));

	return shaderBlob;
}

void ShaderCompiler::OutputCompileErrors(IDxcResult* shaderResult) {
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;

	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError && shaderError->GetStringLength() != 0) {
		LOG_ERROR("HLSLでコンパイルエラーが発生しました。 : {}", shaderError->GetStringPointer());
		assert(false);
	}
}
