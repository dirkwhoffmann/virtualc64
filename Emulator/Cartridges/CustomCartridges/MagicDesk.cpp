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
MagicDesk::resetCartConfig()
{
    trace(CRT_DEBUG, "Starting cartridge in 8K game mode\n");
    expansionport.setCartridgeMode(CRTMODE_8K);
}

u8
MagicDesk::peekIO1(u16 addr)
{
    return spypeekIO1(addr);
}

u8
MagicDesk::spypeekIO1(u16 addr) const
{
    return disabled() ? vic.getDataBusPhi1() : control;
}

void
MagicDesk::pokeIO1(u16 addr, u8 value)
{
    control = value & 0x8F;

    // printf("%x control = %x\n", addr, control);

    /* This cartridge type is very similar to the OCEAN cart type: ROM  memory
     * is  organized  in  8Kb  ($2000)  banks  located  at  $8000-$9FFF.  Bank
     * switching is done by writing the bank number to $DE00. Deviant from the
     * Ocean type, bit 8 is cleared for selecting one of the ROM banks. If bit
     * 8 is set ($DE00 = $80), the GAME/EXROM lines are disabled,  turning  on
     * RAM at $8000-$9FFF instead of ROM.
     */

    expansionport.setExromLine(disabled());
    bankIn(value & 0x0F);
}

u8
MagicDesk::peekIO2(u16 addr)
{
    return spypeekIO2(addr);
}

u8
MagicDesk::spypeekIO2(u16 addr) const
{
    return vic.getDataBusPhi1();
}
