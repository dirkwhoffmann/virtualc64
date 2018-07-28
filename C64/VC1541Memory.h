/*!
 * @header      VC151Memory.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2008 - 2018 Dirk W. Hoffmann
 */
/* This program is free software; you can redistribute it and/or modify
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

#ifndef _VC1541MEMORY_INC
#define _VC1541MEMORY_INC

#include "Memory.h"

class VC1541;

/*! @brief    Represents RAM and ROM of a virtual VC1541 floopy disk drive.
 */
class VC1541Memory : public Memory {

public:
    
	//! @brief    Reference to the connected IEC bus
	// IEC *iec;

	//! @brief    Reference to the connected disk drive
	VC1541 *drive;
	
    //! @brief    Random Access Memory
    uint8_t ram[0x0800];
    
    //! @brief    Read Only Memory
    uint8_t rom[0x4000];
    
public:
	
	//! @brief    Constructor
	VC1541Memory();
	
	//! @brief    Destructor
	~VC1541Memory();

	//! @brief    Method from VirtualComponent
	void reset();
		
	//! @brief    Method from VirtualComponent
	void dumpState();
		
	//! @brief    Returns true iff the ROM image has been loaded.
    bool romIsLoaded() { return rom[0] != 0x00; }
				
    // Reading from memory
    uint8_t peek(uint16_t addr);
    uint8_t peekZP(uint8_t addr) { return ram[addr]; }

    // Snooping in memory (no side effects)
    uint8_t spypeek(uint16_t addr);
    
    // Writing into memory
    void poke(uint16_t addr, uint8_t value);
    void pokeZP(uint8_t addr, uint8_t value) { ram[addr] = value; }
};

#endif
