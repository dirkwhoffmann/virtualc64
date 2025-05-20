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

//
// Enumerations
//

/// Parallel cable type
enum ParCableType : long
{
    NONE,         ///< No parallel cable attached
    STANDARD,     ///< Standard parallel cable
    DOLPHIN3      ///< DolphinDOS cable
};

struct ParCableTypeEnum : util::Reflection<ParCableTypeEnum, ParCableType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(ParCableType::DOLPHIN3);

    static const char *_key(ParCableType value)
    {
        switch (value) {

            case ParCableType::NONE:     return "NONE";
            case ParCableType::STANDARD: return "STANDARD";
            case ParCableType::DOLPHIN3: return "DOLPHIN3";
        }
        return "???";
    }
    
    static const char *help(ParCableType value)
    {
        return "";
    }
};

}
