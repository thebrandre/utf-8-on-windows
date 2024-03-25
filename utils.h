#pragma once
#include <algorithm>
#include <bit>
#include <cstdint>
#include <format>
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
  const auto lc_firstByte = std::bit_cast<std::uint8_t>(*UTF8EncodedBytes);
  const auto getNextByteValue = [](char f_char) -> std::uint32_t {
    if ((std::bit_cast<std::uint8_t>(f_char) & 0b0100'0000u) != 0u)
      throw std::runtime_error("invalid next byte");
    return std::bit_cast<std::uint8_t>(f_char) & ~0b1000'0000u;
  };

  const auto lc_numberOfBytes = std::countl_one(lc_firstByte);
  if (lc_numberOfBytes == 0)
    return lc_firstByte;
  if (lc_numberOfBytes == 2)
    return (static_cast<std::uint32_t>(lc_firstByte & ~0b1100'0000u) << 6) +
           getNextByteValue(UTF8EncodedBytes[1]);
  if (lc_numberOfBytes == 3)
    return (static_cast<std::uint32_t>(lc_firstByte & ~0b1110'0000u) << 12) +
           (getNextByteValue(UTF8EncodedBytes[1]) << 6) +
           getNextByteValue(UTF8EncodedBytes[2]);
  if (lc_numberOfBytes == 4)
    return (static_cast<std::uint32_t>(lc_firstByte & ~0b1111'0000u) << 18) +
           (getNextByteValue(UTF8EncodedBytes[1]) << 12) +
           (getNextByteValue(UTF8EncodedBytes[2]) << 6) +
           getNextByteValue(UTF8EncodedBytes[3]);

  throw std::runtime_error("invalid start byte");
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
// static_assert(vu::getCodePoint("\xF8") == 0xF8);
