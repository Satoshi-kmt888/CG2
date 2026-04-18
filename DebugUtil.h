#pragma once

#include <string>

struct _EXCEPTION_POINTERS;

//ログファイルの初期化
void InitLog();

//ログ
void Log(const std::string& message);

//
void Log(std::ostream& os, const std::string& message);

//クラッシュ検知
LONG WINAPI ExportDump(_EXCEPTION_POINTERS* exception);
