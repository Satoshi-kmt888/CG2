#pragma once
#include <string>
#include <Windows.h>

//ログ
void Log(const std::string& message);

//クラッシュ検知
LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);
