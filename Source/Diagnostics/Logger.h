#pragma once

#include <fstream>
#include <format>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>

class Logger {
public:
	//--- インナークラス ---

	//ログの出力レベル
	enum class LogLevel {
		Info,    //通知
		Warning, //警告
		Error    //エラー
	};

	//--- インスタンス管理 ---

	Logger() = delete;
	~Logger() = delete;

	//--- 公開関数 ---

	/// <summary>
	/// ログフォルダおよびログファイルを生成・
	/// </summary>
	static void Initialize();

	/// <summary>
	/// ログファイルを閉じる
	/// </summary>
	static void Finalize() noexcept;

	/// <summary>
	/// 基本的なログ出力
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="level">出力レベル</param>
	/// <param name="location"></param>
	/// <param name="fmt"></param>
	/// <param name="...args"></param>
	template <class... Args>
	static void Log(LogLevel level, const std::source_location& location, std::format_string<Args...> fmt, Args&&... args) {
		LogInternal(level, location, std::format(fmt, std::forward<Args>(args)...));
	}

private:

	/// <summary>
	/// ログ出力の内部処理
	/// </summary>
	/// <param name="level"></param>
	/// <param name="location"></param>
	/// <param name="message"></param>
	static void LogInternal(LogLevel level, const std::source_location& location, const std::string& message);

	/// <summary>
	/// 現在時刻のstring変換
	/// </summary>
	/// <returns>現在時刻の文字列</returns>
	static std::string GetCurrentTimeString();

	/// <summary>
	/// 出力レベルのstring変換
	/// </summary>
	/// <param name="level">出力レベル</param>
	/// <returns>出力レベルの文字列</returns>
	static constexpr std::string_view ToString(LogLevel level) {
		switch (level) {
			using enum Logger::LogLevel;
		case Info: return "INFO";
		case Warning: return "WARNING";
		case Error: return "ERROR";
		default: return "UNKNOWN";
		}
	}

	//--- 内部変数 ---

	static inline std::ofstream logStream_;
	static inline std::mutex mutex_;
	static inline bool isInitialize_ = false;
};

#define LOG_INFO(fmt, ...) do{Logger::Log(Logger::LogLevel::Info, std::source_location::current(), fmt, ##__VA_ARGS__);} while(0)
#define LOG_WARNING(fmt, ...) do{Logger::Log(Logger::LogLevel::Warning, std::source_location::current(), fmt, ##__VA_ARGS__);} while(0)
#define LOG_ERROR(fmt, ...) do{Logger::Log(Logger::LogLevel::Error, std::source_location::current(), fmt, ##__VA_ARGS__);} while(0)
