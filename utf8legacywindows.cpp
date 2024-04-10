#include "utils.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <print>
#include <ranges>

#include <Windows.h>

int main() {
  std::print("Active Code Page: {}\tOEM code page: {}\tConsoleOutputCP: {}\n",
             GetACP(), GetOEMCP(), GetConsoleOutputCP());
  SetConsoleOutputCP(CP_UTF8);
  // UTF-8 support via setlocale requires Windows 10 version 1803 (10.0.17134.0)
  auto DefaultLocale = std::locale::global(std::locale("en_US.UTF-8"));
  std::print("Default locale: {}\n", DefaultLocale.name());

  std::print("Active Code Page: {}\tOEM code page: {}\tConsoleOutputCP: {}\n",
             GetACP(), GetOEMCP(), GetConsoleOutputCP());

  const auto UTF8Literal = u8"\u0041 \u00E4\u00F6\u00FC\u00DF \u03BC \u2030 "
                           u8"\u0C20 \U0001F606\U0001F431";
  const auto UTF8Literal2 = u8"A äöüß μ ‰ ఠ 😆🐱";
  const auto UTF8Literal3 =
      "A \xc3\xa4\xc3\xb6\xc3\xbc\xc3\x9f \xce\xbc \xe2\x80\xb0 \xe0\xb0\xa0 "
      "\xf0\x9f\x98\x86\xf0\x9f\x90\xb1";
  std::print("Input as Unicode code points with \\u and \\U: {}\n",
             reinterpret_cast<const char *>(UTF8Literal));
  std::print("Direct: {}\n", reinterpret_cast<const char *>(UTF8Literal2));
  std::print("Explicit UTF-8 encoded values: {}\n", UTF8Literal3);
  assert(std::u8string_view(UTF8Literal) == std::u8string_view(UTF8Literal2));
  assert(std::u8string_view(UTF8Literal) ==
         std::u8string_view(reinterpret_cast<const char8_t *>(UTF8Literal3)));
  {
    const auto Message = std::string("WriteConsoleA: ") + UTF8Literal3 + "\n";
    DWORD CharactersWritten = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), Message.c_str(), Message.size(),
                  &CharactersWritten, nullptr);
    assert(Message.size() == CharactersWritten);
  }

  {
    LPCWSTR Message = L"WriteConsoleW: A äöüß μ ‰ ఠ 😆🐱\n";
    auto MessageLength = static_cast<DWORD>(std::wstring_view{Message}.size());
    DWORD CharactersWritten = 0;
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), Message, MessageLength,
                  &CharactersWritten, nullptr);
    assert(MessageLength == CharactersWritten);
  }

  {
    const auto OutputFilePath =
        std::filesystem::current_path() / L"file-with-unicode-characters.txt";
    std::print("Print to {}\n", OutputFilePath.generic_string());
    std::fstream OutFile(OutputFilePath, std::ios::out);
    vu::printDefaultBOM(OutFile)
        << reinterpret_cast<const char *>(UTF8Literal) << "\n"
        << reinterpret_cast<const char *>(UTF8Literal2) << "\n"
        << UTF8Literal3 << "\n";
    std::print(OutFile, "{}", reinterpret_cast<const char *>(UTF8Literal));
  }

  {
    static_assert(vu::getCodePoint("😆") == 0x1F606);
    // mojibake without std::locale("en_US.UTF-8")
    // const auto OutputFilePath = std::filesystem::current_path() /
    // "file-with-unicode-characters\U0001F606.txt";

    // both variants throw an error without std::locale("en_US.UTF-8")
    const auto OutputFilePath = std::filesystem::current_path() /
                                u8"file-with-unicode-characters\U0001F606.txt";
    // const auto OutputFilePath = std::filesystem::current_path() /
    // L"file-with-unicode-characters\U0001F606.txt";

    std::print("Print to {}\n", OutputFilePath.generic_string());
    std::basic_fstream<char8_t> OutFile(OutputFilePath, std::ios::out);
    vu::printDefaultBOM(OutFile) << UTF8Literal << "\n" << UTF8Literal2 << "\n";

    // does not (yet) compile with Visual Studio 17.9.4
    // std::print(OutFile, u8"{}", UTF8Literal);
    // std::print(OutFile, "{}", UTF8Literal);
  }

  return 0;
}
