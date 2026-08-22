// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "vcconfig.h"
#include "utl/abilities/Loggable.h"
#include <type_traits>

#ifndef NDEBUG

namespace vc64::debug {

/* The flag descriptor tables, generated from the X-macro lists in vcdebug.h.
 * Each entry wraps its flag in a pair of captureless lambdas, so that flags
 * of different types can be read and written through a common interface.
 */

#define LOG_FLAG_ENTRY(name, dflt, help) \
    { #name, help, false, \
      []() -> long { return (long)name; }, \
      [](long value) { name = LogLevel(value); } },

#define DEBUG_FLAG_ENTRY(type, name, dflt, help) \
    { #name, help, std::is_same_v<type, bool>, \
      []() -> long { return (long)name; }, \
      [](long value) { name = (type)value; } },

const std::vector<FlagInfo> logFlags = { VC_LOG_FLAGS(LOG_FLAG_ENTRY) };
const std::vector<FlagInfo> debugFlags = { VC_DEBUG_FLAGS(DEBUG_FLAG_ENTRY) };

#undef LOG_FLAG_ENTRY
#undef DEBUG_FLAG_ENTRY

}

#endif
