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

/// Cartridge mode
enum_long(CRTMODE)
{
    CRTMODE_16K,        //! 16KB cartridge
    CRTMODE_8K,         //! 8KB cartridge
    CRTMODE_ULTIMAX,    //! Ultimax cartridge
    CRTMODE_OFF         //! No cartridge
};
typedef CRTMODE CRTMode;

struct CRTModeEnum : util::Reflection<CRTModeEnum, CRTMode> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CRTMODE_OFF;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "CRTMODE"; }
    static const char *key(long value)
    {
        switch (value) {

            case CRTMODE_16K:      return "16K";
            case CRTMODE_8K:       return "8K";
            case CRTMODE_ULTIMAX:  return "ULTIMAX";
            case CRTMODE_OFF:      return "OFF";
        }
        return "???";
    }
};

}
