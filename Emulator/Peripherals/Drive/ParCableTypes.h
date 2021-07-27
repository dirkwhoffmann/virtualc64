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

//
// Enumerations
//

enum_long(PAR_CABLE_TYPE)
{
    PAR_CABLE_NONE,
    PAR_CABLE_SPEEDDOS,
    PAR_CABLE_DOLPHIN2,
    PAR_CABLE_DOLPHIN3
};
typedef PAR_CABLE_TYPE ParCableType;

#ifdef __cplusplus
struct ParCableTypeEnum : util::Reflection<ParCableTypeEnum, ParCableType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= PAR_CABLE_DOLPHIN3;
    }
    
    static const char *prefix() { return "PAR_CABLE"; }
    static const char *key(ParCableType value)
    {
        switch (value) {
                
            case PAR_CABLE_NONE:     return "PAR_CABLE_NONE";
            case PAR_CABLE_SPEEDDOS: return "PAR_CABLE_SPEEDDOS";
            case PAR_CABLE_DOLPHIN2: return "PAR_CABLE_DOLPHIN2";
            case PAR_CABLE_DOLPHIN3: return "PAR_CABLE_DOLPHIN3";
        }
        return "???";
    }
};
#endif
