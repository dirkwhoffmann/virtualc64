#include "Powerplay.h"

void
Powerplay::poke(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDE00 && addr <= 0xDFFF);
    
    /*
     * Bank switching is done by writing to $DE00.
     *
     * Bit in DE00 -> 76543210
     *                xx210xx3 <- Bit in selected bank number
     *
     * A value of $86 is written to disable the cartridge.
     */

    if (addr == 0xDE00) {
       
        if (value == 0x86) {
            setGameLine(1);
            setExromLine(1);
            return;
        }
        
        uint8_t bank = ((value >> 3) & 0x07) | ((value << 3) & 0x08);
        assert(bank < 16);
        bankIn(bank);
    }
}
