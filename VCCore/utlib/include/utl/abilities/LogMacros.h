// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

/* The logging macros are kept apart from the Loggable class, because macros
 * are visible everywhere. Only implementation files (via the config headers)
 * and the few headers that log in inline code include this file, so that the
 * public headers of the emulator cores do not export them.
 */

#pragma once

#include "utl/abilities/Loggable.h"

#ifdef logmsg
#undef logmsg
#endif

/* In debug builds, CONSTEXPR expands to nothing. If the key is one of the
 * constant severities (e.g., LOG_WARN), the condition below is then a
 * compile-time constant and MSVC reports warning C4127. The condition is
 * deliberate, so the warning is silenced for this macro only.
 */
#ifdef _MSC_VER
#define UTL_SUPPRESS_CONSTANT_CONDITION_BEGIN __pragma(warning(push)) __pragma(warning(disable: 4127))
#define UTL_SUPPRESS_CONSTANT_CONDITION_END   __pragma(warning(pop))
#else
#define UTL_SUPPRESS_CONSTANT_CONDITION_BEGIN
#define UTL_SUPPRESS_CONSTANT_CONDITION_END
#endif

#define logmsg(key, format, ...) \
    do { \
        UTL_SUPPRESS_CONSTANT_CONDITION_BEGIN \
        if CONSTEXPR (key != LOG_OFF) \
            log(key, std::source_location::current(), \
                format __VA_OPT__(,) __VA_ARGS__); \
        UTL_SUPPRESS_CONSTANT_CONDITION_END \
    } while (0)

#define xfiles(format, ...) \
    logmsg(LOG_XFILES, format __VA_OPT__(,) __VA_ARGS__)
