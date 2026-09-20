#include "Debugger/CrashHandler.h"

#include "Debugger/Logger.h"

#include <Windows.h>
#include <array>
#include <minidumpapiset.h>
#include <strsafe.h>

#pragma comment(lib, "DbgHelp.lib")

namespace {
	LONG __stdcall ExportDump(_EXCEPTION_POINTERS* exception) {
		//時刻を取得して、自国を名前に入れたファイルを作成。Dumpsディレクトリ以下に主力
		SYSTEMTIME time;
		GetLocalTime(&time);
		std::array<wchar_t, MAX_PATH> filePath = { 0 };

		CreateDirectory(L"./Dumps", nullptr);
		StringCchPrintfW(
			filePath.data(), MAX_PATH,
			L"./Dumps/%04d-%02d%02d-%02d%02d.dmp",
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute
		);

		if (HANDLE dumpFileHandle = CreateFile(
			filePath.data(),
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,
			nullptr, CREATE_ALWAYS, 0, nullptr
		); dumpFileHandle != INVALID_HANDLE_VALUE) {
			//processId(このexeのId)とクラッシュ(例外)の発生したthreadIdを取得
			DWORD processId = GetCurrentProcessId();
			DWORD threadId = GetCurrentThreadId();

			//設定情報を入力
			MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
			minidumpInformation.ThreadId = threadId;
			minidumpInformation.ExceptionPointers = exception;
			minidumpInformation.ClientPointers = TRUE;

			//Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
			MiniDumpWriteDump(
				GetCurrentProcess(), processId, dumpFileHandle,
				MiniDumpNormal, &minidumpInformation, nullptr, nullptr
			);

			CloseHandle(dumpFileHandle);
		}


		//他に関連付けられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
		return EXCEPTION_EXECUTE_HANDLER;
	}
} //namespace

void CrashHandler::Register() {
	// OSに例外ハンドラを登録
	SetUnhandledExceptionFilter(ExportDump);
	LOG_INFO("CrashHandler registered successfully.");
}
