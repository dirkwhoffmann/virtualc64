/*!
 * @header      Kingsoft.h
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

#ifndef _KINGSOFT_INC
#define _KINGSOFT_INC

#include "Cartridge.h"

class Kingsoft : public Cartridge {
    
public:
    
    Kingsoft(C64 *c64) : Cartridge(c64, "Kingsoft") { };
    CartridgeType getCartridgeType() { return CRT_KINGSOFT; }
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    void resetCartConfig();

    uint8_t peekIO1(uint16_t addr);
    uint8_t spyPeek(uint16_t addr) { return 0; }
    void pokeIO1(uint16_t addr, uint8_t value);

    void updatePeekPokeLookupTables();
};



#endif
