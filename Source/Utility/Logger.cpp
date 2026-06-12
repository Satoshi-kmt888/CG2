#include "Logger.h"

#include <array>
#include <cassert>
#include <chrono>
#include <excpt.h>
#include <filesystem>
#include <format>
#include <memory>
#include <minidumpapiset.h>
#include <strsafe.h>

#pragma comment(lib, "Dbghelp.lib")

Logger& Logger::GetInstance() {
	static Logger instance;
	return instance;
}

void Logger::Initialize() {
	std::lock_guard lock(mutex_);

	//ログのディレクトリを用意
	std::filesystem::create_directory("logs");

	//現在時刻取得(UTC時刻)
	auto now = std::chrono::system_clock::now();
	//ログファイルの名前を秒までの時刻にする
	auto nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	//日本時間(PCの設定時間)に変換
	std::chrono::zoned_time localTime{ std::chrono::current_zone(), nowSeconds };
	//formatで年月日_時分秒に変換
	std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
	//時刻からファイル名を決定
	std::string logFilePath = "logs/" + dateString + ".log";
	//ファイルを使って書き込み準備
	logStream_.open(logFilePath);
	assert(logStream_.is_open());
}

void Logger::LogInternal(const std::string& message) {
	std::lock_guard lock(mutex_);

	if (logStream_.is_open()) {
		logStream_ << message << '\n';
	}

	OutputDebugStringA((message + "\n").c_str());
}

void Logger::Finalize() {
	std::lock_guard lock(mutex_);

	if (logStream_.is_open()) {
		logStream_.flush();
		logStream_.close();
	}
}

namespace Debug {
	LONG __stdcall ExportDump(_EXCEPTION_POINTERS* exception) {
		//時刻を取得して、自国を名前に入れたファイルを作成。Dumpsディレクトリ以下に主力
		SYSTEMTIME time;
		GetLocalTime(&time);

		std::array<wchar_t, MAX_PATH> filePath = { 0 };
		CreateDirectory(L"./Dumps", nullptr);
		StringCchPrintfW(filePath.data(), MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
		HANDLE rawHandle = CreateFile(filePath.data(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);

		if (rawHandle == INVALID_HANDLE_VALUE) {
			return EXCEPTION_EXECUTE_HANDLER;
		}

		auto deleter = [](HANDLE h) {
			if (h && h != INVALID_HANDLE_VALUE) ::CloseHandle(h);
			};
		std::unique_ptr<void, decltype(deleter)> dumpFileHandle(rawHandle, deleter);

		//processId(このexeのId)とクラッシュ(例外)の発生したthreadIdを取得
		DWORD processId = GetCurrentProcessId();
		DWORD threadId = GetCurrentThreadId();

		//設定情報を入力
		MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
		minidumpInformation.ThreadId = threadId;
		minidumpInformation.ExceptionPointers = exception;
		minidumpInformation.ClientPointers = TRUE;

		//Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
		MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle.get(), MiniDumpNormal, &minidumpInformation, nullptr, nullptr);

		//他に関連付けられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
		return EXCEPTION_EXECUTE_HANDLER;
	}
}
