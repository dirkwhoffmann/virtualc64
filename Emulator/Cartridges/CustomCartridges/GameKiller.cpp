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
GameKiller::resetCartConfig()
{
    debug(CRT_DEBUG, "Starting GameKiller cartridge in NOCART mode\n");

    control = 0;
    expansionport.setCartridgeMode(CRTMODE_OFF);
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
        expansionport.setCartridgeMode(CRTMODE_OFF);
    }
}

void
GameKiller::pokeIO2(u16 addr, u8 value)
{
    debug(CRT_DEBUG, "GameKiller::pokeIO2(%x, %d)\n", addr, control);
    
    if (++control > 1) {
        expansionport.setCartridgeMode(CRTMODE_OFF);
    }
}

const string
GameKiller::getButtonTitle(isize nr) const
{
    return nr == 1 ? "Freeze" : "";
}

void
GameKiller::pressButton(isize nr)
{
    if (nr == 1) {
        c64.signalExpPortNmi();
    }
}

void
GameKiller::releaseButton(isize nr)
{
    if (nr == 1) {

        SUSPENDED

        cpu.releaseNmiLine(INTSRC_EXP);
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
    expansionport.setCartridgeMode(CRTMODE_OFF);
}

