// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

enum_long(MEMACCESS)
{
    MEMACCESS_R,     // Memory Refresh
    MEMACCESS_I,     // Idle read
    MEMACCESS_C,     // Character access
    MEMACCESS_G,     // Graphics access
    MEMACCESS_P,     // Sprite pointer access
    MEMACCESS_S,     // Sprite data access
    MEMACCESS_COUNT
};
typedef MEMACCESS MemAccess;

#ifdef __cplusplus
struct MemAccessEnum : util::Reflection<MemAccessEnum, MemAccess> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = MEMACCESS_S;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }
    
    static const char *prefix() { return "MEMACCESS"; }
    static const char *key(MemAccess value)
    {
        switch (value) {
                
            case MEMACCESS_R:      return "R";
            case MEMACCESS_I:      return "I";
            case MEMACCESS_C:      return "C";
            case MEMACCESS_G:      return "G";
            case MEMACCESS_P:      return "P";
            case MEMACCESS_S:      return "S";
            case MEMACCESS_COUNT:  return "???";
        }
        return "???";
    }
};
#endif
