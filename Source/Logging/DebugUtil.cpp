#include <Windows.h>
#include <strsafe.h>
#include <dbghelp.h>
#include <fstream>
#include <chrono>
#include <filesystem>

#include "Logging/DebugUtil.h"

#pragma comment(lib, "Dbghelp.lib")

static std::ofstream gLogStream;

void InitializeLog() {
	//ログのディレクトリを用意
	std::filesystem::create_directory("Projects/logs");

	//現在時刻取得(UTC時刻)
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	//ログファイルの名前を秒までの時刻にする
	std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
		nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	//日本時間(PCの設定時間)に変換
	std::chrono::zoned_time localTime{ std::chrono::current_zone(), nowSeconds };
	//formatで年月日_時分秒に変換
	std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
	//時刻からファイル名を決定
	std::string logFilePath = std::string("Projects/logs/") + dateString + ".log";
	//ファイルを使って書き込み準備
	gLogStream.open(logFilePath);
}

void Log(const std::string& message) {
	gLogStream << message << std::endl;
	OutputDebugStringA(message.c_str());
}

void FinalizeLog() {
	if (gLogStream.is_open()) {
		gLogStream.close();
	}
}

LONG __stdcall ExportDump(_EXCEPTION_POINTERS* exception) {
	//時刻を取得して、自国を名前に入れたファイルを作成。Dumpsディレクトリ以下に主力
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	//processId(このexeのId)とクラッシュ(例外)の発生したthreadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	//設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	//Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	//他に関連付けられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する

	return EXCEPTION_EXECUTE_HANDLER;
}
