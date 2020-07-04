// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

void
Kingsoft::resetCartConfig()
{
    // Start in 16KB game mode by reading from I/O space 1
    (void)peekIO1(0);
}

void
Kingsoft::updatePeekPokeLookupTables()
{
    // Tweak lookup tables if we run in Ultimax mode
    if (vc64.getUltimax()) {
        
        // $0000 - $7FFF and $C000 - $DFFF are usable the normal way
        u8 exrom = 0x10;
        u8 game  = 0x08;
        u8 index = (pport.read() & 0x07) | exrom | game;

        for (unsigned bank = 0x1; bank <= 0x7; bank++) {
            MemoryType type = mem.bankMap[index][bank];
            mem.peekSrc[bank] = mem.pokeTarget[bank] = type;
        }
        for (unsigned bank = 0xC; bank <= 0xD; bank++) {
            MemoryType type = mem.bankMap[index][bank];
            mem.peekSrc[bank] = mem.pokeTarget[bank] = type;
        }
    }
}

u8
Kingsoft::peekIO1(u16 addr)
{
    // Switch to 16KB game mode
    expansionport.setCartridgeMode(CRT_16K);
 
    // Bank in second packet to ROMH
    bankInROMH(1, 0x2000, 0);
    
    return 0;
}

void
Kingsoft::pokeIO1(u16 addr, u8 value)
{
    // Switch to (faked) Ultimax mode
    expansionport.setCartridgeMode(CRT_ULTIMAX);
    
    // Bank in third packet to ROMH
    bankInROMH(2, 0x2000, 0);
}

