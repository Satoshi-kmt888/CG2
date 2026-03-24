#pragma once
#include <string>

struct IDxcBlob;
struct IDxcUtils;
struct IDxcCompiler3;
struct IDxcIncludeHandler;

//シェーダをコンパイル
IDxcBlob* CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile,
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler
);
