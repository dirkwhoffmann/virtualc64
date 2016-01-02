/*!
 * @header      Datasette.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2016 Dirk W. Hoffmann
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

// Forward declarations
class CPU;

class Memory : public VirtualComponent {

public:

	// --------------------------------------------------------------------------------
	//                                Constant definitions
	// --------------------------------------------------------------------------------
	
	/*! @brief    Memory type
	 *  @details  This datatype defines a constant value for the different kinds of memory.
     */
	enum MemoryType {
		MEM_RAM,
		MEM_ROM,
		MEM_IO
	};
	
	//! @brief    Returns true, if the specified address is a valid memory address.
	virtual bool isValidAddr(uint16_t addr, MemoryType type) = 0;
	
	//! @brief    Reference to the connected virtual CPU
	CPU *cpu; 

public:
	
	//! @brief    Constructor
	Memory();
	
	//! @brief    Destructor
	~Memory();

    
	// --------------------------------------------------------------------------------
	//                                    Peek
	// --------------------------------------------------------------------------------

protected:	

	//! @brief    Reads a byte from RAM.
	virtual uint8_t peekRam(uint16_t addr) = 0;
	
    //! @brief    Reads a byte from ROM.
	virtual uint8_t peekRom(uint16_t addr) = 0;

    //! @brief    Reads a byte from I/O space.
	virtual uint8_t peekIO(uint16_t addr) = 0;

public:

	//! @brief    Reads a byte from the specified memory source.
	uint8_t peekFrom(uint16_t addr, MemoryType source);

	/*! @brief    Reads a byte from memory.
	 *  @details  This function implements the native memory peek of the original C64 including all side effects.
     */
	virtual uint8_t peek(uint16_t addr) = 0;
	
	//! Wrapper around peek
	/*!	Memory address is provided in LO/HIBYTE format.
		\param lo Low-Byte of the memory address 
		\param hi High-Byte of the memory address
		\return RAM, ROM, or I/O memory contents at address (hi << 8) + lo 
	*/
	inline uint8_t peek(uint8_t lo, uint8_t hi) { return peek(((uint16_t)hi << 8) + lo); }

	//! Wrapper around peek
	/*!	Memory address is provided in three parts.
		\param lo Low-Byte of the memory address 
		\param hi High-Byte of the memory address
		\param index The effective address is computed by adding index
		\return RAM, ROM, or I/O memory contents at address (hi << 8) + lo + index 
	*/
	inline uint8_t peek(uint8_t lo, uint8_t hi, uint8_t index) { return peek(((uint16_t)hi << 8) + lo + index); }

	//! Wrapper around peek
	/*!	Returns 16 bit value at the provided address
		The functions first determines the correct memory source by the value of the processor port register.
		Afterwards, the value is read either from RAM, ROM, or the I/O address space and returned.
		\param addr Memory Addr
		\return RAM, ROM, or I/O memory contents at addresses addr, addr+1
		\remark The C64 is a little endian machine. Hence, the low byte comes first. 
	*/	
	inline uint16_t peekWord(uint16_t addr) { return ((uint16_t)peek(addr+1) << 8) | peek(addr); }


	// --------------------------------------------------------------------------------
	//                                    Poke
	// --------------------------------------------------------------------------------

protected:	

	//! @brief    Writes a byte into RAM
    virtual void pokeRam(uint16_t addr, uint8_t value) = 0;
	
    //! @brief    Writes a byte into ROM
	virtual void pokeRom(uint16_t addr, uint8_t value) = 0;

    //! @brief    Writes a byte into I/O space
	virtual void pokeIO(uint16_t addr, uint8_t value) = 0;
	
public:
	
	//! Write a BYTE to the specified memory.
	/*! Depending in the specified memory type, either \a pokeRam, \a pokeRom, or \a pokeIO is called. */
	void pokeTo(uint16_t addr, uint8_t value, MemoryType dest);

	//! Write a BYTE to memory.
	/*! This functions implements the native memory poke of the original C64 including all side effects.
	 The functions automatically determines the correct memory source by the value of the processor 
	 port register. Afterwards, the value is written either to RAM, ROM, or the I/O address space and.
	 \param addr Memory address
	 \param value Value to write
	 */
	virtual void poke(uint16_t addr, uint8_t value) = 0;

	//! Wrapper around poke
	/*!	Memory address is provided in LO/HIBYTE format.
		\param lo Low-Byte of the memory address 
		\param hi High-Byte of the memory address
		\param value Value to write
	*/
	inline void poke(uint8_t lo, uint8_t hi, uint8_t value) 
		{ poke(((uint16_t)hi << 8) + lo, value); }  

	//! Wrapper around peek
	/*!	Memory address is provided in three parts.
		\param lo Low-Byte of the memory address 
		\param hi High-Byte of the memory address
		\param index The effective address is computed by adding index
		\param value Value to write
	*/
	inline void poke(uint8_t lo, uint8_t hi, uint8_t index, uint8_t value) 
		{ poke((uint16_t)(hi << 8) + lo + index, value); }

	//! Wrapper around peek
	/*!	Write 16 bit value to the provided address
		\param addr Memory Addr
		\param value Value to write
		\remark The C64 is a little endian machine. Hence, the low byte comes first. 
	*/	
	inline void pokeWord(uint16_t addr, uint16_t value)
		{ poke (addr, LO_BYTE(value)); poke (addr+1, HI_BYTE(value)); }

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
