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

void
FreezeFrame::_reset(bool hard)
{
    // In Ultimax mode, the same ROM chip that appears in ROML also appears
    // in ROMH. By default, it it appears in ROML only, so let's bank it in
    // ROMH manually.
    bankInROMH(0, 0x2000, 0);
}

u8
FreezeFrame::peekIO1(u16 addr)
{
    // Reading from IO1 switched to 8K game mode
    expansionPort.setCartridgeMode(CRTMODE_8K);
    return 0;
}

u8
FreezeFrame::spypeekIO1(u16 addr) const
{
    return 0;
}

u8
FreezeFrame::peekIO2(u16 addr)
{
    // Reading from IO2 disables the cartridge
    expansionPort.setCartridgeMode(CRTMODE_OFF);
    return 0;
}

u8
FreezeFrame::spypeekIO2(u16 addr) const
{
    return 0;
}

const char *
FreezeFrame::getButtonTitle(isize nr) const
{
    return nr == 1 ? "Freeze" : "";
}

void
FreezeFrame::pressButton(isize nr)
{
    if (nr == 1) {

        expansionPort.setCartridgeMode(CRTMODE_ULTIMAX);
        cpu.pullDownNmiLine(INTSRC_EXP);
    }
}

void
FreezeFrame::releaseButton(isize nr)
{
    if (nr == 1) {

        cpu.releaseNmiLine(INTSRC_EXP);
    }
}

}
