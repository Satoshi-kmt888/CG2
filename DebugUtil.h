#pragma once

#include <string>

struct _EXCEPTION_POINTERS;

//ログ
void Log(const std::string& message);

//クラッシュ検知
LONG WINAPI ExportDump(_EXCEPTION_POINTERS* exception);
