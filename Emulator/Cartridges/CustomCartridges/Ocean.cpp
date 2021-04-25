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

/*
 * For more information: http://codebase64.org/doku.php?id=base:crt_file_format
 *
 * "Memory is divided into 8Kb ($2000) banks. For the lower 128Kb, memory is
 *  banked into $8000-$9FFF and for the upper 128Kb, memory is banked into
 *  $A000-$BFFF. Note that the Terminator 2 cartridge loads all 64 banks at
 *  $8000-$9FFF.
 *
 *  Bank switching is done by writing to $DE00. The lower six bits give the bank
 *  number (ranging from 0-63). Bit 8 in this selection word is always set."
 */

Ocean::Ocean(C64 &ref) : Cartridge(ref)
{
    msg("Ocean::Ocean\n");
};

/*
bool
Ocean::getGameLineInCrtFile() const
{
    msg("Ocean::getGameLineInCrtFile()\n");
    return 1;
}
*/

void
Ocean::pokeIO1(u16 addr, u8 value)
{
    msg("Ocean: pokeIO1(%x,%x)\n", addr, value);
    
    if (addr == 0xDE00) {
        bankIn(value & 0x3F);
    }
}
