#include <filesystem>
#include <format>
#include <fstream>
#include <string_view>

#include "utils.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main() {
#ifdef _WIN32
  std::print(
      "Active code page: {}\nOEM code page: {}\nConsole output code page: {}\n",
      GetACP(), GetOEMCP(), GetConsoleOutputCP());
#endif

  const std::filesystem::path OutputPath =
      std::filesystem::current_path() / "ω😆.txt";
  std::fstream File(OutputPath, std::fstream::out | std::fstream::trunc);
  std::print(File, "Wingdings are still a thing! 📫👓🖉☯\n");

  std::print("File created? {}\n",
             std::filesystem::exists(OutputPath) ? "💪" : "💩");

  const std::filesystem::path OutputPathUpperCase =
      std::filesystem::current_path() / "Ω😆.txt";
  std::print("Greek is case-sensitive? {}\n",
             !std::filesystem::exists(OutputPathUpperCase) ? "✓" : "💔");
  std::print("Paths are {}equivalent! \n",
             OutputPath == OutputPathUpperCase ? "" : "NOT ");

  for (std::string_view View : {"µ", "a", "🐱", "\uFEFF"})
    vu::describeCodePoint(View);

  static_assert(std::string_view("\u00B5") == std::string_view("µ"));
  static_assert(std::string_view("\u03BC") == std::string_view("μ"));
  static_assert(std::string_view("µ") != std::string_view("μ"));

  constexpr auto MoralDilemma =
      "How do I spell my name now, Andr\u00E9, Andre\u0301, or Andre\u02CA?\n";
  std::print(File, MoralDilemma);
  std::print(MoralDilemma);
  static_assert(std::string_view("é") == std::string_view("\u00E9"));
}
