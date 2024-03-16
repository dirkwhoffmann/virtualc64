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

#include "config.h"
#include "VICII.hpp"
#include "C64.hpp"

namespace vc64 {

ScreenGeometry
VICII::getScreenGeometry(void) const
{
    unsigned rows = GET_BIT(reg.current.ctrl1, 3) ? 25 : 24;
    unsigned cols = GET_BIT(reg.current.ctrl2, 3) ? 40 : 38;
    
    if (cols == 40) {
        return rows == 25 ? SCREEN_GEOMETRY_25_40 : SCREEN_GEOMETRY_24_40;
    } else {
        return rows == 25 ? SCREEN_GEOMETRY_25_38 : SCREEN_GEOMETRY_24_38;
    }
}

}
