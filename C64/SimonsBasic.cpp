//
//  SimonsBasic.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 21.01.18.
//

#include "SimonsBasic.h"

/*
 * From: http://codebase64.org/doku.php?id=base:crt_file_format
 *
 * "SIMONS BASIC permanently uses 16 kB ($4000) bytes of cartridge memory from
 *  $8000-$BFFF. However, through some custom bank-switching logic the upper area
 *  ($A000-$BFFF) may be disabled so Simons BASIC may use it as additional RAM.
 *
 *  The CRT file contains two CHIP blocks of length $2000 each, the first block
 *  having a start address of $8000, the second block $A000. In the cartridge header,
 *  EXROM ($18) is set to 0, GAME ($19) is set to 1 to indicate the RESET/power-up
 *  configuration of 8 kB ROM."
 *
 *  From http://www.syntiac.com/pdf/chameleon_progmanual_beta9.pdf
 *
 *  "A write action to DE00h switches to 16K mode, while a read at DE00h switches
 *   to 8K mode. After reset the cartridge emulation is in 16 KByte mode."
 */
void
SimonsBasic::powerup()
{
    bankIn(0);
    bankIn(1);
}

uint8_t
SimonsBasic::peekIO(uint16_t addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDFFF);
    
    if (addr == 0xDE00) {
        // Switch to 8KB configuration
        setGameLine(1);
    }
    return Cartridge::peek(addr);
}

void
SimonsBasic::poke(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDE00 && addr <= 0xDFFF);
 
    if (addr == 0xDE00) {
        // Switch to 16KB configuration
        setGameLine(0);
    }
}
