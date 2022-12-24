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
Reu::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)

    if (!getBattery()) {
        trace(CRT_DEBUG, "Erasing REU contents\n");
        eraseRAM(0);
    } else {
        trace(CRT_DEBUG, "Preserving REU contents\n");
    }
}

u8
Reu::peekIO1(u16 addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    debug(REU_DEBUG,"peekIO1(%x)\n", addr);

    return 0;
}

u8
Reu::spypeekIO1(u16 addr) const
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    debug(REU_DEBUG,"spypeekIO1(%x)\n", addr);

    return 0;
}

u8
Reu::peekIO2(u16 addr)
{
    debug(REU_DEBUG,"peekIO2(%x)\n", addr);

    return 0;
}

u8
Reu::spypeekIO2(u16 addr) const
{
    debug(REU_DEBUG,"spypeekIO2(%x)\n", addr);

    return 0;
}

void
Reu::pokeIO1(u16 addr, u8 value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    debug(REU_DEBUG,"pokeIO1(%x,%x)\n", addr, value);
}

void
Reu::pokeIO2(u16 addr, u8 value)
{
    debug(REU_DEBUG,"peekIO2(%x,%x)\n", addr, value);
}
