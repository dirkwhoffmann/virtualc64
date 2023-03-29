// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "VICII.h"
#include "C64.h"

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
