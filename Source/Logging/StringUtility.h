#pragma once

#include <string>

namespace StringUtility {
	/// <summary>
	/// string->wstring変換
	/// </summary>
	/// <param name="str">文字列</param>
	/// <returns>wstringに変換された文字列</returns>
	std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// wstring->string変換
	/// </summary>
	/// <param name="str">文字列</param>
	/// <returns>string型に変換された文字列</returns>
	std::string ConvertString(const std::wstring_view& str);
} //namespace StringUtility
