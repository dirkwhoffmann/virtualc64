/*!
 * @header      StarDos.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _STARDOS_INC
#define _STARDOS_INC

#include "Cartridge.h"

class StarDos : public Cartridge {
    
    private:
    
    uint64_t voltage = 5000000;
    uint64_t latestVoltageUpdate = 0;
    
    public:
    
    StarDos(C64 *c64) : Cartridge(c64, "StarDos") { };
    CartridgeType getCartridgeType() { return CRT_STARDOS; }
    void reset();
    
    void updateVoltage();
    void charge();
    void discharge();
    void enableROML();
    void disableROML();

    void updatePeekPokeLookupTables(); 
    uint8_t peekIO1(uint16_t addr) { charge(); return 0; }
    uint8_t peekIO2(uint16_t addr) { discharge(); return 0; }
    void pokeIO1(uint16_t addr, uint8_t value) { charge(); }
    void pokeIO2(uint16_t addr, uint8_t value) { discharge(); }
    
    bool hasResetButton() { return true; }
};

#endif
