// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"

void
WarpSpeed::resetCartConfig()
{
    expansionport.setCartridgeMode(CRTMODE_16K);
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
    expansionport.setCartridgeMode(CRTMODE_16K);
}

void
WarpSpeed::pokeIO2(u16 addr, u8 value)
{
    expansionport.setCartridgeMode(CRTMODE_OFF);
}

const char *
WarpSpeed::getButtonTitle(isize nr) const
{
    return nr == 1 ? "Reset" : nullptr;
}

void
WarpSpeed::pressButton(isize nr)
{
    assert(nr <= numButtons());

    suspend();
    resetWithoutDeletingRam();
    resume();
}
