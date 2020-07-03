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
Supergames::pokeIO2(u16 addr, uint8_t value)
{
    /* Bits 0, 1: Bank bits 0 and 1
     * Bits 2:    Exrom / Game control
     * Bits 3:    if 1, further writes to DE00 have no effect (not implemented)
     */
    
    if (addr == 0xDF00) {
        
        if (value & 0x04) {
            c64->expansionport.setCartridgeMode(CRT_8K);
        } else {
            c64->expansionport.setCartridgeMode(CRT_16K);
        }
        
        bankIn(value & 0x03);
    }
}
