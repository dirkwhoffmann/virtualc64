/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
 *
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

// Last review: 25.7.06

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
	
	//! Memory type
	/*! This datatype defines a constant value for the different kinds of memory. */
	enum MemoryType {
		MEM_RAM,
		MEM_ROM,
		MEM_IO
	};
	
	//! Returns true, if the specified address is a valid memory address
	/*! Note, that the ROM and IO memory only partially cover the address space. */
	virtual bool isValidAddr(uint16_t addr, MemoryType type) = 0;
	
	//! Reference to the connected virtual CPU
	CPU *cpu; 

public:
	
	//! Constructor
	Memory();
	
	//! Destructor
	~Memory();

	// --------------------------------------------------------------------------------
	//                                    Peek
	// --------------------------------------------------------------------------------

protected:	

	//! Read a BYTE from RAM.
	/*! The BYTE is always read from RAM, regardless of the value of the processor port register.
		\param addr Memory address
		\return RAM contents at address addr
		\see peek 
	*/
	virtual uint8_t peekRam(uint16_t addr) = 0;
	
	//! Read a BYTE from ROM.
	/*! The BYTE is always read from ROM, regardless of the value of the processor port register.
		\param addr Memory address
		\return ROM contents at address addr
		\see peek 
	*/
	virtual uint8_t peekRom(uint16_t addr) = 0;

	//! Read a BYTE from I/O space.
	/*! The BYTE is always read from I/O space, regardless of the value of the processor port register.
		\param addr Memory address
		\return I/O register contents at address addr
		\see peek 
	*/
	virtual uint8_t peekIO(uint16_t addr) = 0;

public:

	//! Read a BYTE from the specified memory source.
	/*! Depending in the specified memory type, either \a peekRam, \a peekRom, or \a peekIO is called. */
	uint8_t peekFrom(uint16_t addr, MemoryType source);

	//! Read a BYTE from memory.
	/*! This functions implements the native memory peek of the original C64 including all side effects.
	 The functions automatically determines the correct memory source by the value of the processor 
	 port register. Afterwards, the value is read either from RAM, ROM, or the I/O address space and 
	 returned.
	 \param addr Memory address
	 \return RAM, ROM, or I/O memory contents at address addr 
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

	//! Write a BYTE to RAM.
	/*! The BYTE is always written to RAM, regardless of the value of the processor port register.
		\param addr Memory address
		\param value Value to write
		\see poke 
	*/
	virtual void pokeRam(uint16_t addr, uint8_t value) = 0;
	
	//! Write a BYTE to ROM.
	/*! The BYTE is always written to ROM, regardless of the value of the processor port register.
		\param addr Memory address
		\param value Value to write
		\see poke 
	*/
	virtual void pokeRom(uint16_t addr, uint8_t value) = 0;

	//! Write a BYTE to I/O space.
	/*! The BYTE is always written to I/O space, regardless of the value of the processor port register.
		\param addr Memory address
		\param value Value to write
		\see poke 
	*/
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
