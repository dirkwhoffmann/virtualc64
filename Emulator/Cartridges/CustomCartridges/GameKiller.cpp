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
#include "Emulator.h"

namespace vc64 {

void
GameKiller::resetCartConfig()
{
    debug(CRT_DEBUG, "Starting GameKiller cartridge in NOCART mode\n");
    
    control = 0;
    expansionPort.setCartridgeMode(CRTMODE_OFF);
}

u8
GameKiller::peek(u16 addr)
{
    return Cartridge::peek(addr);
}

void
GameKiller::pokeIO1(u16 addr, u8 value)
{
    debug(CRT_DEBUG, "GameKiller::pokeIO1(%x, %d)\n", addr, control);
    
    if (++control > 1) {
        expansionPort.setCartridgeMode(CRTMODE_OFF);
    }
}

void
GameKiller::pokeIO2(u16 addr, u8 value)
{
    debug(CRT_DEBUG, "GameKiller::pokeIO2(%x, %d)\n", addr, control);
    
    if (++control > 1) {
        expansionPort.setCartridgeMode(CRTMODE_OFF);
    }
}

const char *
GameKiller::getButtonTitle(isize nr) const
{
    return nr == 1 ? "Freeze" : "";
}

void
GameKiller::pressButton(isize nr)
{
    if (nr == 1) {
        emulator.put(CMD_CPU_NMI, 1);
    }
}

void
GameKiller::releaseButton(isize nr)
{
    if (nr == 1) {
        emulator.put(CMD_CPU_NMI, 0);
    }
}

void
GameKiller::updatePeekPokeLookupTables()
{
    debug(CRT_DEBUG, "updatePeekPokeLookupTables\n");
    
    if (control <= 1) {
        
        mem.peekSrc[0xE] = M_CRTHI;
        mem.peekSrc[0xF] = M_CRTHI;
    }
}

void
GameKiller::nmiWillTrigger()
{
    debug(CRT_DEBUG, "nmiWillTrigger");
    
    control = 0;
    expansionPort.setCartridgeMode(CRTMODE_OFF);
}

}
