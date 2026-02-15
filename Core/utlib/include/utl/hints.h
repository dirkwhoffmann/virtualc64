/// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include <cassert>

//
// Optimizing code
//

#if defined(__clang__)

#define unreachable    __builtin_unreachable()
#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)
#define alwaysinline   __attribute__((always_inline))

#elif defined(__GNUC__) || defined(__GNUG__)

#define unreachable    __builtin_unreachable()
#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)
#define alwaysinline   __attribute__((always_inline))

#elif defined(_MSC_VER)

#define unreachable    __assume(false)
#define likely(x)      (x)
#define unlikely(x)    (x)
#define alwaysinline   __forceinline

#elif

#define unreachable
#define likely(x)
#define unlikely(x)
#define alwaysinline inline

#endif

#define fatalError     assert(false); unreachable


//
// Debugging
//

/* The following macro can be used to disable clang sanitizer checks. It has
 * been added to make the code compatible with gcc which doesn't recognize
 * the 'no_sanitize' keyword.
 */
#if defined(__clang__)
#define NO_SANITIZE(x) __attribute__((no_sanitize(x)))
#else
#define NO_SANITIZE(x)
#endif
