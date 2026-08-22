// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Reflectable.h"

namespace retro::vault {

using utl::isize;
using utl::u8;

//
// Enumerations
//

enum class Platform : long { AMIGA, C64 };

struct PlatformEnum : utl::Reflectable<PlatformEnum, Platform> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Platform::C64);

    static const char *_key(Platform value)
    {
        switch (value) {

            case Platform::AMIGA: return "AMIGA";
            case Platform::C64:   return "C64";
        }
        return "???";
    }
};

}
