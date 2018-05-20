/*!
 * @header      Memory.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2018 Dirk W. Hoffmann
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

#ifndef _MEMORY_INC
#define _MEMORY_INC

#include "basic.h"
#include "VirtualComponent.h"
#include "Memory_types.h"

// Forward declarations
class CPU;

class Memory : public VirtualComponent {

    friend CPU;
    
public:
	
	//! @brief    Constructor
	Memory();
	
	//! @brief    Destructor
	~Memory();

    //
    //! @functiongroup Reading from memory
    //

private:
    
	/*! @brief    Peeks a byte from memory.
	 *  @details  This function is called by the CPU to read values from memory.
     *            The peek source table determines where the value comes from.
     *  @seealso  spy()
     */
	virtual uint8_t peek(uint16_t addr) = 0;
	
	//! @brief    Convenience wrapper for peek
    uint8_t peek(uint8_t lo, uint8_t hi) { return peek(LO_HI(lo, hi)); }

public:
    
    //! @brief    Same as peek, but without side effects
    virtual uint8_t spy(uint16_t addr) = 0;
    
    
    //
    //! @functiongroup Writing into memory
    //
    
private:

	//! @brief    Writes a byte into RAM
    virtual void pokeRam(uint16_t addr, uint8_t value) = 0;
	
    //! @brief    Writes a byte into ROM
	virtual void pokeRom(uint16_t addr, uint8_t value) = 0;
	
public:
	
    /*! @brief    Pokes a byte into memory.
     *  @details  This function is called by the CPU to write into memory.
     *            The poke target table determines where the value is written to.
     */
	virtual void poke(uint16_t addr, uint8_t value) = 0;

	//! Load a ROM image into memory.
	/*! All bytes of the specified file are read into the ROM memory, starting at the specified location.
	   The function is unsafe, i.e., it does not check if the file is a valid ROM file or if the address 
	   is adequate. Hence, call checkRomFile prior to this function. 
	   \param filename Name of the file being loaded
	   \param start Start address in ROM memory 
	*/
	void flashRom(const char *filename, uint16_t start);
};

#endif
