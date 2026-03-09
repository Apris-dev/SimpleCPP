#pragma once

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