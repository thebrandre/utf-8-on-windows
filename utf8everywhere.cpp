#include "utils.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <print>
#include <string_view>

#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifdef _WIN32
	std::print("Active Code Page: {}\nOEM code page: {}\n", GetACP(), GetOEMCP());
#endif

	const std::filesystem::path OutputPath = std::filesystem::current_path() / "ω😆.txt";
	std::fstream File(OutputPath, std::fstream::out | std::fstream::trunc);
	std::print(File, "Wingdings are still a thing! 📫👓🖉☯\n");
	std::print(File, "How do I spell my name now, Andr\u00E9, Andre\u0301, or Andre\u02CA?\n");
	static_assert(std::string_view("é") == std::string_view("\u00E9"));

	std::print("File created? {}\n", std::filesystem::exists(OutputPath) ? "💪" : "💩");

	const std::filesystem::path OutputPathUpperCase = std::filesystem::current_path() / "Ω😆.txt";
	std::print("Greek is case-insensitive on Windows, too? {}\n",
			   !std::filesystem::exists(OutputPathUpperCase) ? "✓" : "💔");

	for (std::string_view View : {"µ", "a", "🐱", "\uFEFF"})
	{
		std::print("Code point {:#x} is rendered as {} and encoded with {} character(s) as {} or {}.\n",
				   vu::getCodePoint(View.data()), View, View.size(), vu::formatAsHexadecimalValues(View),
				   vu::formatAsBinaryValues(View));
	}

	static_assert(std::string_view("\u00B5") == std::string_view("µ"));
	static_assert(std::string_view("\u03BC") == std::string_view("μ"));
	static_assert(std::string_view("µ") != std::string_view("μ"));
}
