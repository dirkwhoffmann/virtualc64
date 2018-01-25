//
//  FinalIII.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 21.01.18.
//

#include "C64.h"

void
FinalIII::powerup()
{
    debug("FinalCartridge::powerup\n");

    /* Final cartridge III contains four 16KB ROMs residing at $8000 - $BFFF
     *
     *     Bank 0:  BASIC, Monitor, Disk-Turbo
     *     Bank 1:  Notepad, BASIC (Menu Bar)
     *     Bank 2:  Desktop, Freezer/Print
     *     Bank 3:  Freezer, Compression
     *
     * Final cartrige III switches frequently betwenn different exrom/game line
     * configurations. I.e., it uses ultimax mode to override the NMI vectors
     * stored in Kernel ROM. Switching between configurations causes ROMH sometimes
     * to be visible at $A000 and sometimes at $E000. As we do not want to copy memory
     * back and forth, we simply add four new chips at location $E000 which
     * reflect the upper half of the original four cartridge chips.
     */
    for (unsigned i = 0; i < 4; i++) {
        
        assert(chip[i] != NULL);
        chipStartAddress[i+4] = 0xE000;
        chipSize[i+4]         = 0x2000;
        chip[i+4]             = (uint8_t *)malloc(0x2000);
        memcpy(chip[i+4], chip[i] + 0x2000, 0x2000);
    }
    
    // c64->cpu.setNMILineExpansionPort();
    bankIn(0);
    setGameLine(0);
    setExromLine(0);
}

uint8_t
FinalIII::peekIO(uint16_t addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDFFF);
    
    if (addr == 0xDFFF) {
        debug("Peeking from 0xDFFF\n");
    }
              
    // The I/O space mirrors $1E00 to $1EFF from the selected bank.
    uint16_t offset = addr - 0xDE00;
    return peek(0x8000 + 0x1E00 + offset);
}

void
FinalIII::poke(uint16_t addr, uint8_t value) {

    assert(addr >= 0xDE00 && addr <= 0xDFFF);
    
    // 0xDFFF is Final Cartridge's internal control register
    if (addr == 0xDFFF) {
        
        /*  "7      Hide this register (1 = hidden)
         *   6      NMI line   (0 = low = active) *1)
         *   5      GAME line  (0 = low = active) *2)
         *   4      EXROM line (0 = low = active)
         *   2-3    unassigned (usually set to 0)
         *   0-1    number of bank to show at $8000
         *
         *   1) if either the freezer button is pressed,
         *      or bit 6 is 0, then an NMI is generated
         *
         *   2) if the freezer button is pressed, GAME
         *      is also forced low" [VICE]
         */
        
        uint8_t hide  = value & 0x80;
        uint8_t nmi   = value & 0x40;
        uint8_t game  = value & 0x20;
        uint8_t exrom = value & 0x10;
        uint8_t bank  = value & 0x03;
        
        debug("hide: %d nmi:%d game:%d exrom:%d bank:%d\n", hide != 0, nmi != 0, game != 0, exrom != 0, bank);
        
        // Bit 7
        if (hide) {
            setGameLine(1);
            setExromLine(1);
        }
        
        // Bit 6
        nmi ? c64->cpu.clearNMILineExpansionPort()
            : c64->cpu.setNMILineExpansionPort();

        // Bit 5 and 4
        setGameLine(game);
        setExromLine(exrom);

        // Bit 1 and 0
        bankIn(bank);
        bankIn(bank+4);
    }
}

void
FinalIII::pressButton() {

    debug("FinalIII:pressFreeze\n");

    // The freezer is enabled by selecting bank 0 in unimax mode and triggering an NMI
    poke(0xDFFF, 0x10);
}

/*
void
FinalIII::triggerNMI() {
    debug("FinalIII:triggerNMI\n");
    c64->cpu.setNMILineExpansionPort();
}
*/
