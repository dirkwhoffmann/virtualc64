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
/// @file

#pragma once

#include "Reflection.h"

namespace vc64 {

enum class MemAccess : long
{
    R,     // Memory Refresh
    I,     // Idle read
    C,     // Character access
    G,     // Graphics access
    P,     // Sprite pointer access
    S,     // Sprite data access
    COUNT
};

struct MemAccessEnum : util::Reflection<MemAccessEnum, MemAccess> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(MemAccess::S);

    static const char *_key(MemAccess value)
    {
        switch (value) {

            case MemAccess::R:      return "R";
            case MemAccess::I:      return "I";
            case MemAccess::C:      return "C";
            case MemAccess::G:      return "G";
            case MemAccess::P:      return "P";
            case MemAccess::S:      return "S";
            case MemAccess::COUNT:  return "???";
        }
        return "???";
    }
    
    static const char *help(MemAccess value)
    {
        return "";
    }
};

}
