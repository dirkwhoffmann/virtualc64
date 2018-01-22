//
//  FinalIII.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 21.01.18.
//

#include "C64.h"

/* More information:
 * ftp://www.zimmers.net/pub/cbm/documents/chipdata/Final%20Cartridge%20III%20Internals.txt
 *
 * From VICE:
 * "- one register at $DFFF:
 *
 *   7      Hide this register (1 = hidden)
 *   6      NMI line   (0 = low = active) *1)
 *   5      GAME line  (0 = low = active) *2)
 *   4      EXROM line (0 = low = active)
 *   2-3    unassigned (usually set to 0)
 *   0-1    number of bank to show at $8000
 *
 *   1) if either the freezer button is pressed, or bit 6 is 0, then
 *   an NMI is generated
 *
 *   2) if the freezer button is pressed, GAME is also forced low
 *
 *  This implementation also supports the community developed "Final Cartridge III+"
 *  which has 16 ROM banks instead of the usual 4."
 */

void
FinalIII::powerup()
{
    debug("FinalCartridge::powerup\n");
    
    bankIn(0);
    
    setGameLine(0);
    setExromLine(0);
    // c64->cpu.setNMILineCIA();
    
    // c64->expansionport.gameLineHasChanged();
    // c64->expansionport.exromLineHasChanged();

}

uint8_t
FinalIII::peekIO(uint16_t addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDFFF);
    
    uint16_t offset = addr - 0xDE00;
    
    // The I/O space mirrors $1E00 to $1EFF from the selected bank.
    uint8_t result = rom[0x1E00 + offset];
    
    /*
    if (addr == 0xDFFF) {
        debug("FinalCartridge::peek %04X (%02X)\n", addr, result);
    }
    */
    
    return result;
}

void
FinalIII::poke(uint16_t addr, uint8_t value) {

    assert(addr >= 0xDE00 && addr <= 0xDFFF);
    
    // debug("FinalCartridge::poke %04X\n", addr);
    
    if (addr == 0xDFFF) {
        
        // debug("Final Cartridge III: Writing %02X to $DFFF\n", value);
        
        uint8_t hide  = value & 0x80;
        uint8_t nmi   = value & 0x40;
        uint8_t game  = value & 0x20;
        uint8_t exrom = value & 0x10;
        uint8_t bank  = value & 0x03;
        
        // debug("hide: %d nmi:%d game:%d exrom:%d bank:%d\n", hide != 0, nmi != 0, game != 0, exrom != 0, bank);

        if (hide) {
            debug("Disabling final cartridge III\n");
            setGameLine(1);
            setExromLine(1);
        }
        
        bankIn(bank);
        
        setGameLine(game);
        setExromLine(exrom);
        
        if (nmi)
            triggerNMI();
    }
}

void
FinalIII::pushReset() {
        
}
    
void
FinalIII::pushFreeze() {

    triggerNMI();
}

void
FinalIII::triggerNMI() {
        
}
