/*!
 * @header      DriveMemory.h
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

#ifndef _VC1541MEMORY_INC
#define _VC1541MEMORY_INC

#include "Memory.h"

class VC1541;

/*! @brief    Represents RAM and ROM of a virtual VC1541 floopy disk drive.
 */
class VC1541Memory : public Memory {
    
    private:
    
	//! @brief    Reference to the connected disk drive
	VC1541 *drive;
	
    public:
    
    //! @brief    Random Access Memory
    uint8_t ram[0x0800];
    
    //! @brief    Read Only Memory
    uint8_t rom[0x4000];
    
    
    //
    //! @functiongroup Creating and destructing
    //
    
	//! @brief    Constructor
	VC1541Memory(VC1541 *drive);
	
	//! @brief    Destructor
	~VC1541Memory();

    
    //
    //! @functiongroup Methods from VirtualComponent
    //

	void reset();
	void dump();

    
    //
    //! @functiongroup Accessing ROM
    //

	//! @brief    Returns true iff the ROM image has been loaded.
    bool romIsLoaded() { return (rom[0] | rom[1]) != 0x00; }

    //! @brief    Removes the ROM image from memory
    void deleteRom() { memset(rom, 0, sizeof(rom)); }

    /*! @brief    Computes a 64-bit fingerprint for the VC1541 ROM.
     *  @return   fingerprint or 0, if no Basic ROM is installed.
     */
    uint64_t romFingerprint() {
        return romIsLoaded() ? fnv_1a(rom, sizeof(rom)) : 0; }
    
    
    //
    //! @functiongroup Accessing RAM
    //

    // Reading from memory
    uint8_t peek(uint16_t addr);
    uint8_t peekZP(uint8_t addr) { return ram[addr]; }

    // Reading from memory without side effects
    uint8_t spypeek(uint16_t addr);
    
    // Writing into memory
    void poke(uint16_t addr, uint8_t value);
    void pokeZP(uint8_t addr, uint8_t value) { ram[addr] = value; }
};

#endif
