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
#include "C64.h"

namespace vc64 {

u8
Rex::peekIO2(u16 addr)
{
    // Any read access to $DF00 - $DFBF disables the ROM
    if (addr >= 0xDF00 && addr <= 0xDFBF) {
        expansionPort.setCartridgeMode(CRTMODE_OFF);
    }

    // Any read access to $DFC0 - $DFFF switches to 8KB configuration
    if (addr >= 0xDFC0 && addr <= 0xDFFF) {
        expansionPort.setCartridgeMode(CRTMODE_8K);
    }

    return 0;
}

u8
Rex::spypeekIO2(u16 addr) const
{
    return 0;
}

}
