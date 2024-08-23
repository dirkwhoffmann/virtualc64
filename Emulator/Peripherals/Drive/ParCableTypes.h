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
enum_long(PAR_CABLE_TYPE)
{
    PAR_CABLE_NONE,         ///< No parallel cable attached
    PAR_CABLE_STANDARD,     ///< Standard parallel cable
    PAR_CABLE_DOLPHIN3      ///< DolphinDOS cable
};
typedef PAR_CABLE_TYPE ParCableType;

struct ParCableTypeEnum : util::Reflection<ParCableTypeEnum, ParCableType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = PAR_CABLE_DOLPHIN3;

    static const char *prefix() { return "PAR_CABLE"; }
    static const char *_key(long value)
    {
        switch (value) {

            case PAR_CABLE_NONE:     return "PAR_CABLE_NONE";
            case PAR_CABLE_STANDARD: return "PAR_CABLE_STANDARD";
            case PAR_CABLE_DOLPHIN3: return "PAR_CABLE_DOLPHIN3";
        }
        return "???";
    }
};

}
