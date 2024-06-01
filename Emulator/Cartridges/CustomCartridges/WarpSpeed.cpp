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
WarpSpeed::resetCartConfig()
{
    expansionPort.setCartridgeMode(CRTMODE_16K);
}

u8
WarpSpeed::peekIO1(u16 addr)
{
    return Cartridge::peekRomL(0x1E00 | (addr & 0xFF));
}

u8
WarpSpeed::spypeekIO1(u16 addr) const
{
    return Cartridge::spypeekRomL(0x1E00 | (addr & 0xFF));
}

u8
WarpSpeed::peekIO2(u16 addr)
{
    return Cartridge::peekRomL(0x1F00 | (addr & 0xFF));
}

u8
WarpSpeed::spypeekIO2(u16 addr) const
{
    return Cartridge::spypeekRomL(0x1F00 | (addr & 0xFF));
}

void
WarpSpeed::pokeIO1(u16 addr, u8 value)
{
    expansionPort.setCartridgeMode(CRTMODE_16K);
}

void
WarpSpeed::pokeIO2(u16 addr, u8 value)
{
    expansionPort.setCartridgeMode(CRTMODE_OFF);
}

const char *
WarpSpeed::getButtonTitle(isize nr) const
{
    return nr == 1 ? "Reset" : "";
}

void
WarpSpeed::pressButton(isize nr)
{
    assert(nr <= numButtons());

    c64.softReset();
}

}
