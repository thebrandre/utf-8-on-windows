#pragma once
#include <algorithm>
#include <bit>
#include <cstdint>
#include <format>
#include <print>
#include <ranges>

namespace vu {
template <typename T>
concept U8CompatibleChar = std::integral<T> && sizeof(T) == 1;

template <typename T>
concept U16CompatibleChar = std::integral<T> && sizeof(T) == 2;

/// @note These methods would be trivial if the current implementation of
/// std::format had all the fmt features,
///	which is not yet the case in Visual Studio 17.9.4 even with C++23
/// enabled.

/// @{
auto formatAsHexadecimalValues(std::ranges::input_range auto &&CharArray) {
  return std::ranges::fold_left(
      CharArray | std::views::transform([](U8CompatibleChar auto f_char) {
        return std::format("\\{:#x}", std::bit_cast<std::uint8_t>(f_char));
      }),
      "", std::plus{});
}

auto formatAsBinaryValues(std::ranges::input_range auto &&CharArray) {
  return std::ranges::fold_left_first(
             CharArray |
                 std::views::transform([](U8CompatibleChar auto f_char) {
                   return std::format("{:0>8b}",
                                      std::bit_cast<std::uint8_t>(f_char));
                 }),
             [](std::string f_left, std::string f_right) {
               return std::move(f_left) + " " + std::move(f_right);
             })
      .value_or("");
}
/// @}

/// @brief implementation of Python's `ord` builtin.
constexpr std::uint32_t
getCodePoint(const U8CompatibleChar auto *UTF8EncodedBytes) {
  const auto FirstByte = std::bit_cast<std::uint8_t>(*UTF8EncodedBytes);

  const auto NumberOfLeadingOneBits = std::countl_one(FirstByte);
  if (NumberOfLeadingOneBits == 0)
    return FirstByte; // 1 byte ASCII character ∈[0x0,07F]

  // Note: Unicode reserves code points up to \U0010FFFF (1,114,112 characters).
  // So [0xF5, 0xF8] are invalid start bytes as well.
  if (NumberOfLeadingOneBits == 1 ||
      NumberOfLeadingOneBits > 4) // ∈[0x80,0xBF]∪[0xF8,0xFF]
    throw std::runtime_error("invalid start byte");

  // decompose leading byte into length and value information
  const auto NumberOfBytes = NumberOfLeadingOneBits;

  const auto getContinuationByteValue =
      [](U8CompatibleChar auto f_char) -> std::uint32_t {
    if ((std::bit_cast<std::uint8_t>(f_char) & 0b0100'0000u) != 0u)
      throw std::runtime_error("invalid continuation byte"); // ∉ [0x80,0xBF]
    return std::bit_cast<std::uint8_t>(f_char) & ~0b1000'0000u;
  };

  // start byte ∈[0xC0,0xDF] but valid is only [0xC2,0xDF]
  if (NumberOfBytes == 2) {
    const auto FirstByteValue =
        static_cast<std::uint32_t>(FirstByte) & 0b0001'1111u;
    const std::uint32_t CodePoint =
        (FirstByteValue << 6) + getContinuationByteValue(UTF8EncodedBytes[1]);
    static_assert(0x80u == 1u << 7);
    if (CodePoint < 0x80) // overlong sequence that uses only 7 bits
      throw std::runtime_error("invalid start byte");
    return CodePoint;
  }

  // start byte ∈[0xE0,0xEF]
  if (NumberOfBytes == 3) {
    const auto FirstByteValue =
        static_cast<std::uint32_t>(FirstByte) & 0b0000'1111u;
    const std::uint32_t CodePoint =
        (FirstByteValue << 12) +
        (getContinuationByteValue(UTF8EncodedBytes[1]) << 6) +
        getContinuationByteValue(UTF8EncodedBytes[2]);
    static_assert(0x800u == 1u << 11);
    if (CodePoint < 0x800u) // overlong sequence that uses only 11 bits
      throw std::runtime_error("invalid start byte");
    static_assert(0xDFFFu - 0xD800u + 1u == 2048u);
    if (CodePoint >= 0xD800u && CodePoint <= 0xDFFFu)
      throw std::runtime_error("code point within the surrogate range");
    return CodePoint;
  }

  // start byte ∈[0xF0,0xF7]
  if (NumberOfBytes == 4) {
    const auto FirstByteValue =
        static_cast<std::uint32_t>(FirstByte) & 0b0000'0111u;
    const std::uint32_t CodePoint =
        (FirstByteValue << 18) +
        (getContinuationByteValue(UTF8EncodedBytes[1]) << 12) +
        (getContinuationByteValue(UTF8EncodedBytes[2]) << 6) +
        getContinuationByteValue(UTF8EncodedBytes[3]);
    static_assert(0x10000u == 1u << 16);
    if (CodePoint < 0x10000u) // overlong sequence that uses only 16 bits
      throw std::runtime_error("invalid start byte");
    return CodePoint;
  }

#ifdef __cpp_lib_unreachable
  std::unreachable();
#else
  __builtin_unreachable();
#endif
}

template <U8CompatibleChar T_Char, typename T_Traits>
auto printDefaultBOM(std::basic_ostream<T_Char, T_Traits> &OutputStream)
    -> std::basic_ostream<T_Char, T_Traits> & {
  return OutputStream << "\xEF\xBB\xBF";
}

template <U16CompatibleChar T_Char, typename T_Traits>
auto printDefaultBOM(std::basic_ostream<T_Char, T_Traits> &OutputStream)
    -> std::basic_ostream<T_Char, T_Traits> & {
  // UTF-16 (LE) is the default encoding of the Windows Kernel
  return OutputStream << L"\xFFFE";
}

// @return true iff the file has a Byte Order Mark (BOM)
template <U8CompatibleChar T_Char, typename T_Traits>
bool swallowBOM(std::basic_istream<T_Char, T_Traits> &InputStream) {
  if (InputStream.peek() != 0xEF)
    return false;
  InputStream.get();
  if (InputStream.peek() != 0xBB) {
    InputStream.unget();
    return false;
  }
  InputStream.get();
  if (InputStream.peek() != 0xBF) {
    InputStream.unget();
    InputStream.unget();
    return false;
  }
  InputStream.get();
  return true;
}

template <U8CompatibleChar T_Char>
void describeCodePoint(std::basic_string_view<T_Char> EncodedCodePoint) {
  std::print("Code point {:#x} is rendered as {} and encoded with {} "
             "character(s) as {} or {}.\n",
             vu::getCodePoint(EncodedCodePoint.data()), EncodedCodePoint,
             EncodedCodePoint.size(),
             vu::formatAsHexadecimalValues(EncodedCodePoint),
             vu::formatAsBinaryValues(EncodedCodePoint));
}

} // namespace vu

static_assert(vu::U8CompatibleChar<char>);
static_assert(vu::U8CompatibleChar<unsigned char>);
static_assert(vu::U8CompatibleChar<char8_t>);
static_assert(vu::U8CompatibleChar<std::uint8_t>);
static_assert(!vu::U8CompatibleChar<int>);
static_assert(!vu::U8CompatibleChar<double>);

static_assert(vu::getCodePoint("\uFEFF") == 0xFEFFu);
static_assert(vu::getCodePoint("µ") == 0xB5u);
static_assert(vu::getCodePoint("μ") == 0x03BCu);
static_assert(vu::getCodePoint("💩") == 0x1F4A9u);
static_assert(vu::getCodePoint("☕") == 0x2615u);
static_assert(vu::getCodePoint("\x41") == 0x41u);
static_assert(vu::getCodePoint("A") == 0x41u);
static_assert(vu::getCodePoint("🍛") == 0x1F35Bu);
static_assert(vu::getCodePoint("\u007F") == 0x7Fu);   // largest 1-byte value
static_assert(vu::getCodePoint("\u0080") == 0x80u);   // smallest 2-byte value
static_assert(vu::getCodePoint("\u07FF") == 0x7FFu);  // largest 2-byte value
static_assert(vu::getCodePoint("\u0800") == 0x800);   // smallest 2-byte value
static_assert(vu::getCodePoint("\uFFFF") == 0xFFFFu); // largest 3-byte value
static_assert(vu::getCodePoint("\U00010000") ==
              0x10000u); // smallest 4-byte value
// static_assert(vu::getCodePoint("\xF8") == 0xF8);
// static_assert(vu::getCodePoint("\uD800") == 0xD800);
// static_assert(vu::getCodePoint("\uDFFF") == 0xDFFF);
// static_assert(vu::getCodePoint("\xED\xA0\x80") == 0xD800);
// static_assert(vu::getCodePoint("\xED\xBF\xBF") == 0xDFFF);
