#pragma once

#include <wrl/client.h>
#include <string>
#include <dxcapi.h>

/// <summary>
/// シェーダーコンパイラ
/// </summary>
class ShaderCompiler {
public:
	void Initialize();
	void Finalize();

	//コンパイル処理
	Microsoft::WRL::ComPtr<IDxcBlob> Compile(
		const std::wstring& filePath,
		const wchar_t* profile
	);

private:
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
};
