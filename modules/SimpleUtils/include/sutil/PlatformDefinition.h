#pragma once

#ifndef USING_MSVC
#include "bits/c++config.h"
using nullptr_t = std::nullptr_t;
#ifndef _CONSTEXPR20
#define _CONSTEXPR20 _GLIBCXX20_CONSTEXPR
#endif
#ifndef _CONSTEXPR23
#define _CONSTEXPR23 _GLIBCXX23_CONSTEXPR
#endif
#endif