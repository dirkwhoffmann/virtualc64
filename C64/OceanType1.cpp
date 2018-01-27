//
//  OceanType1.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 21.01.18.
//

#if 0

#include "OceanType1.h"

/*
 * For more information: http://codebase64.org/doku.php?id=base:crt_file_format
 *
 * "Memory is divided into 8Kb ($2000) banks. For the lower 128Kb, memory is banked
 *  into $8000-$9FFF and for the upper 128Kb, memory is banked into $A000-$BFFF.
 *  Note that the Terminator 2 cartridge loads all 64 banks at $8000-$9FFF.
 *
 * Bank switching is done by writing to $DE00. The lower six bits give the bank number
 * (ranging from 0-63). Bit 8 in this selection word is always set."
 */

void
OceanType1::poke(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDE00 && addr <= 0xDFFF);

    if (addr == 0xDE00) {
        bankIn(value & 0x3F);
    }
}

#endif

