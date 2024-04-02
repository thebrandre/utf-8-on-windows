# Code snippets

* [utf8everywhere.cpp on Compiler Explorer](https://godbolt.org/z/d4PPYGxM3)
* [escaping Unicode characters](https://godbolt.org/z/jc896Pan7)
* [Modifier and Combining Marks](https://godbolt.org/z/YKvqoMefo)

# System Code Pages

There are two relevant code pages:
* the OEM code page for use by legacy console applications,
* the ANSI code page for use by legacy GUI applications

There are several ways to get the active code pages:

* `chcp`: *Displays or sets the active code page number.*
  * 437 IBM437	OEM United States
  * 850 ibm850	OEM Multilingual Latin 1; Western European (DOS)
  * 1250 windows-1250	ANSI Central European; Central European (Windows)
  * 65001	utf-8	Unicode (UTF-8)
  * 1200	utf-16 	Unicode UTF-16, little endian byte order (BMP of ISO 10646)
* PowerShell: `Get-WinSystemLocale`
* PowerShell via registry: `Get-ItemProperty HKLM:\SYSTEM\CurrentControlSet\Control\Nls\CodePage | Select-Object OEMCP, ACP`
* Windows C API: `GetACP()`, `GetOEMCP()`, `GetConsoleOutputCP()`

As of Windows Version 1903, an application can easily set its own code page as described
[here](https://learn.microsoft.com/en-us/windows/apps/design/globalizing/use-utf8-code-page).

# Change the case sensitivity of files and directories

If you can't believe that Greek letters are case insensitive as well,
you can enable case-sensitivity for a specific folder and see for yourself.

```
fsutil.exe file setCaseSensitiveInfo <path> enable
```

# Python

```Python
"🐱".encode("utf-8")
```
returns the encoding `b'\xf0\x9f\x90\xb1'`

```Python
hex(ord("🐱"))
```
returns the code point as a hexadecimal value `'0x1f431'`

Conversion of Unicode String to C++23 escaped characters:
```Python
print("".join([f"\\u{{{ord(x):x}}}" for x in "A äöüß μ ‰ ఠ 😆🐱"]))
```

# Compiler Options

msvc's `/utf-8` is shorthand for `/source-charset:utf-8` and `/execution-charset:utf-8`,
where *source-charset* can be omitted if a consistent BOM is present at each source file.
*execution-charset* affects string literals in the binary. Runtime file IO is not affected.

We use an `.editorconfig` file to tell Visual Studio to save the file without the BOM and use the compiler option to set the encoding.


# Resources

* [Code Page Identifiers](https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers)
* [How can I manually determine the CodePage and Locale of the current OS](https://serverfault.com/q/80635)
* [The Absolute Minimum Every Software Developer Absolutely, Positively Must Know About Unicode and Character Sets (No Excuses!)](https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses/)
* [Use UTF-8 code pages in Windows apps](https://learn.microsoft.com/en-us/windows/apps/design/globalizing/use-utf8-code-page)
* [UTF-8 Everywhere](https://utf8everywhere.org)
* [Unicode: Going Down the Rabbit Hole - Peter Bindels - CppCon 2019](https://www.youtube.com/watch?v=SMSmKg1nApM&t=115s)
* [Fast Conversion From UTF-8 with C++, DFAs, and SSE Intrinsics - Bob Steagall - CppCon 2018](https://youtu.be/5FQ87-Ecb-A)
* [Wingdings and Webdings Symbols](https://unicode.org/L2/L2011/11052r-wingding.pdf)
* [Change the case sensitivity of files and directories](https://learn.microsoft.com/en-us/windows/wsl/case-sensitivity#change-the-case-sensitivity-of-files-and-directories)
* [String and character literals](https://learn.microsoft.com/en-us/cpp/cpp/string-and-character-literals-cpp?view=msvc-170)
* [cppreference: String literal](https://en.cppreference.com/w/cpp/language/string_literal)
* [Escape sequences](https://en.cppreference.com/w/cpp/language/escape)
* [Stack Overflow: Properly print utf8 characters in windows console](https://stackoverflow.com/q/10882277)
* [Byte order mark](https://en.wikipedia.org/wiki/Byte_order_mark)
* [Introduction to UTF Series' Articles, Paweł bbkr Pabian](https://dev.to/bbkr/series/23930)
* [Stack Overflow: What is the difference between "combining characters" and "modifier letters"?](https://stackoverflow.com/a/54457724)
* [Microsoft Documentation on setlocale: UTF-8 support](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170)
* [/source-charset](https://learn.microsoft.com/en-us/cpp/build/reference/source-charset-set-source-character-set?view=msvc-170)
* [/execution-charset](https://learn.microsoft.com/en-us/cpp/build/reference/execution-charset-set-execution-character-set?view=msvc-170)
* [Wikipedia: Mojibake](https://en.wikipedia.org/wiki/Mojibake)
* [Combining character](https://en.wikipedia.org/wiki/Combining_character)
* [The Unicode® Standard, Version 15.0 – Core Specification](https://www.unicode.org/versions/Unicode15.0.0/UnicodeStandard-15.0.pdf)

# Characters

* [µ U+00B5 Micro Sign](https://codepoints.net/U+00B5)
* [μ U+03BC Greek Small Letter Mu](https://codepoints.net/U+03BC)
* [U+0301 Combining Acute Accent](https://codepoints.net/U+0301)
* [U+02CA Modifier Letter Acute Accent](https://codepoints.net/U+02CA)
* [🐱 U+1F431 Cat Face](https://codepoints.net/U+1F431)
* [💩 U+1F4A9 Pile of Poo](https://codepoints.net/U+1F4A9)
* [☕ U+2615 Hot Beverage](https://codepoints.net/U+2615)
* [🍛 U+1F35B Curry and Rice](https://codepoints.net/U+1F35B)
* [U+0338 Combining Long Solidus Overlay](https://codepoints.net/U+0338)
* [; U+037E Greek Question Mark](https://codepoints.net/U+037E)
* [🖖 U+1F596 Raised Hand with Part Between Middle and Ring Fingers, "Vulcan Salute"](https://codepoints.net/U+1F596)
* [U+1F3FB Emoji Modifier Fitzpatrick Type-1-2](https://codepoints.net/U+1F596)
* [U+1F3FF Emoji Modifier Fitzpatrick Type-6](https://codepoints.net/U+1F3FF)
* [👍 U+1F44D Thumbs Up Sign](https://codepoints.net/U+1F44D)