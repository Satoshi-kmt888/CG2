#include "Logger.h"

#include "Logging/StringUtility.h"

#include <chrono>
#include <filesystem>
#include <Windows.h>

#pragma comment(lib, "Dbghelp.lib")

void Logger::Initialize() {
	if (isInitialize_) {
		return;
	}

	//ログのディレクトリを用意
	std::filesystem::create_directory("Projects/Logs");

	//ログファイル名を時刻にする(UTC時刻)
	auto now = std::chrono::system_clock::now();
	auto local = std::chrono::zoned_time{
		std::chrono::current_zone(),
		std::chrono::floor<std::chrono::seconds>(now)
	};
	auto filename = std::format("Projects/Logs/{:%Y%m%d_%H%M%S}.log", local);
	logStream_.open(filename, std::ios::out);

	isInitialize_ = true;
}

void Logger::Finalize() noexcept {
	std::scoped_lock lock(mutex_);

	if (!isInitialize_) {
		return;
	}

	if (logStream_.is_open()) {
		logStream_.flush();
		logStream_.close();
	}

	isInitialize_ = false;
}

void Logger::LogInternal(LogLevel level, const std::source_location& location, const std::string& message) {
	std::scoped_lock lock(mutex_);

	if (!isInitialize_) {
		return;
	}

	auto filename = std::filesystem::path(location.file_name()).filename().string();
	auto text = std::format(
		"[{}] [{}] {}({}) {}",
		GetCurrentTimeString(),
		ToString(level),
		filename,
		location.line(),
		message
	);

	if (logStream_.is_open()) {
		logStream_ << text << '\n';
#ifdef _DEBUG
		logStream_.flush();
#endif
	}

	OutputDebugStringA((text + "\n").c_str());
}

std::string Logger::GetCurrentTimeString() {
	auto now = std::chrono::system_clock::now();

	auto local = std::chrono::zoned_time{
		std::chrono::current_zone(),
		std::chrono::floor<std::chrono::milliseconds>(now)
	};

	return std::format("{:%Y-%m-%d %H:%M:%S}", local);
}


