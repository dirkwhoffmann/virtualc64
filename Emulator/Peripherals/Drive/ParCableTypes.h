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

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

enum_long(PAR_CABLE_TYPE)
{
    PAR_CABLE_NONE,
    PAR_CABLE_STANDARD,
    PAR_CABLE_DOLPHIN3
};
typedef PAR_CABLE_TYPE ParCableType;

#ifdef __cplusplus
struct ParCableTypeEnum : util::Reflection<ParCableTypeEnum, ParCableType> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = PAR_CABLE_DOLPHIN3;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }
    
    static const char *prefix() { return "PAR_CABLE"; }
    static const char *key(ParCableType value)
    {
        switch (value) {
                
            case PAR_CABLE_NONE:     return "PAR_CABLE_NONE";
            case PAR_CABLE_STANDARD: return "PAR_CABLE_STANDARD";
            case PAR_CABLE_DOLPHIN3: return "PAR_CABLE_DOLPHIN3";
        }
        return "???";
    }
};
#endif
