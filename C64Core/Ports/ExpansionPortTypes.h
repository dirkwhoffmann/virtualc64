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

#include "BasicTypes.h"

namespace vc64 {

//
// Enumerations
//

/// Cartridge mode
enum class CRTMode : long
{
    MODE16K,    //! 16KB cartridge
    MODE8K,     //! 8KB cartridge
    ULTIMAX,    //! Ultimax cartridge
    OFF         //! No cartridge
};

struct CRTModeEnum : Reflectable<CRTModeEnum, CRTMode> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(CRTMode::OFF);

    static const char *_key(CRTMode value)
    {
        switch (value) {

            case CRTMode::MODE16K:  return "16K";
            case CRTMode::MODE8K:   return "8K";
            case CRTMode::ULTIMAX:  return "ULTIMAX";
            case CRTMode::OFF:      return "OFF";
        }
        return "???";
    }
    
    static const char *help(CRTMode value)
    {
        return "";
    }
};

//
// Structures
//

typedef struct
{
    isize reuSpeed;
}
ExpansionPortConfig;

}
