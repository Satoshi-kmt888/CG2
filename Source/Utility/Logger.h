#pragma once

#include <Windows.h>

#include <fstream>
#include <mutex>
#include <string>

struct _EXCEPTION_POINTERS;

/// @brief ログ出力クラス
class Logger {
public:
	/*--- インスタンス管理 ---*/

	//インスタンスの取得
	static Logger& GetInstance();

	//コピーガード
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	/*--- 公開関数 ---*/

	void Initialize();

	void LogInternal(const std::string& message);

	void Finalize();

private:
	/*--- コンストラクタ・デストラクタ ---*/

	Logger() = default;
	~Logger() = default;

	/*--- 内部変数 ---*/

	std::ofstream logStream_;
	std::mutex mutex_;
};

namespace Debug {
	/// @brief 入力したメッセージを出力
	/// @param message 
	inline void Log(const std::string& message) {
		Logger::GetInstance().LogInternal(message);
	}

	template <typename Args>
	void Log(std::format_string<Args> fmt, Args&& args) {
		Log(std::format(fmt, std::forward<Args>(args)));
	}

	/// @brief クラッシュ検知およびミニダンプ生成
	/// @param exception 
	/// @return 
	LONG WINAPI ExportDump(_EXCEPTION_POINTERS* exception);
}
