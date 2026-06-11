#pragma once

#include <string>
#include <Windows.h>

struct _EXCEPTION_POINTERS;

//ログファイルの初期化
void InitializeLog();

//ログ
void Log(const std::string& message);

//ログファイルの終了処理
void FinalizeLog();

//クラッシュ検知
LONG WINAPI ExportDump(_EXCEPTION_POINTERS* exception);
