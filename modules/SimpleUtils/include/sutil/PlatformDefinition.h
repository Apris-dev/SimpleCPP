#pragma once

/*
 * The Class that helps define platform specific things
 * For example: path separators, line endings, and the size of 'long'
 */

// TODO: support more than linux, windows, and macOS.  Would need help from outside to support mobile platforms and etc.

#ifndef USING_MSVC
#include "bits/c++config.h"
#ifndef _CONSTEXPR20
#define _CONSTEXPR20 _GLIBCXX20_CONSTEXPR
#endif
#ifndef _CONSTEXPR23
#define _CONSTEXPR23 _GLIBCXX23_CONSTEXPR
#endif
#endif

#ifdef USING_WINDOWS
#define PATH_SEPARATOR '\\'
#define LINE_ENDING "\r\n"
#else
#define PATH_SEPARATOR '/'
#define LINE_ENDING "\n"
#endif

#include <cstdint>

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using smallest = uint8;
using largest = std::size_t;

#if CXX_VERSION >= 20
using utf8 = char8_t;
#else
using utf8 = char;
#endif
using utf16 = char16_t;
using utf32 = char32_t;