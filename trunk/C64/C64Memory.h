/*
 * (C) 2008 Dirk W. Hoffmann. All rights reserved.
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

#ifndef _C64MEMORY_INC
#define _C64MEMORY_INC

#include "Memory.h"

// Forward declarations
class VIC;
class SID;
class CIA1;
class CIA2;

//! This class represents the RAM and ROM of a virtual C64
/*! Note that the RAM, ROM, and the I/O space are superposed and therefore share the same locations in memory.
    The contents of memory location 0x0001 determines which kind of memory is currently visible. */
class C64Memory : public Memory {

public:		

	//! Virtual RAM
	/*! All memory cells can be read or written. */
	uint8_t ram[65536];

	//! Virtual ROM
	/*! Only specific memory cells are valid ROM locations. In total, the C64 has three ROMs that
		are located at different addresses in the ROM space. Note, that the ROMs do not span over
		the whole 64k range. Therefore, only some address are valid ROM addresses.
		\see isRomAddr */
	uint8_t rom[65536];

	//! Virtual color RAM
	/*! The color RAM is located in the I/O space, starting at D800 and ending at DBFF
	Only the lower four bits are accessible, the upper four bits are open and can show any value */
	uint8_t colorRam[1024];
	
	//! True if Basic ROM is visible.
	/*! The variable is updated whenever a value is written to memory location 0x0001. */
	bool basicRomIsVisible;
	
	//! True if Character ROM is visible.
	/*! The variable is updated whenever a value is written to memory location 0x0001. */
	bool charRomIsVisible;

	//! True if Kernel ROM is visible.
	/*! The variable is updated whenever a value is written to memory location 0x0001. */
	bool kernelRomIsVisible;

	//! True if the I/O space is visible.
	/*! The variable is updated whenever a value is written to memory location 0x0001. */
	bool IOIsVisible;
	
	//! Reference to the connected VIC chip. 
	/*! Use setVIC to set the value during initialization.
		The virtual memory needs to know about the VIC chip, because it is accessed via memory mapped I/O.
		For example, if a value is poked to an address in the VIC I/O space, it is handed over directly
		to the VIC chip ("poke" fallthrough).
		\warning The variable is write-once. */
	VIC *vic; 

	//! Reference to the connected SID chip. 
	/*! Use setSID to set the value during initialization.
		The virtual memory needs to know about the SID chip, because it is accessed via memory mapped I/O.
		For example, if a value is poked to an address in the SID I/O space, it is handed over directly
		to the SID chip ("poke" fallthrough).
		\warning The variable is write-once. */
	SID *sid; 
	
	//! References to the connected CIA chips. 
	/*! Use setCIA to set the value during initialization.
		The virtual memory needs to know about the CIA chip, because it is accessed via memory mapped I/O.
		For example, if a value is poked to an address in the CIA I/O space, it is handed over directly
		to the CIA chip ("poke" fallthrough).
		\warning The variable is write-once. */
	CIA1 *cia1; 
	CIA2 *cia2;
	
	//! File name of the Character ROM image.
	/*! The file name is set by the loadRom routine. It is saved for further reference, so the ROM can be reloaded any tim e. */
	char *charRomFile;

	//! File name of the Kernel ROM image.
	/*! The file name is set by the loadRom routine. It is saved for further reference, so the ROM can be reloaded any tim e. */
	char *kernelRomFile;

	//! File name of the Basic ROM image.
	/*! The file name is set by the loadRom routine. It is saved for further reference, so the ROM can be reloaded any tim e. */
	char *basicRomFile;	

	//! Check integrity of Basic ROM image 
	/*! Returns true, iff the specified file contains a valid BASIC ROM image.
	    File integrity is checked via the checkFileHeader function. 
	    \param filename Name of the file being loaded
		\see C64:loadRom
	*/
	static bool isBasicRom(const char *filename);

	//! Check integrity of Kernel ROM image 
	/*! Returns true, iff the specified file contains a valid Kernel ROM image.
		File integrity is checked via the checkFileHeader function.
		@param filename Name of the file being loaded
	*/
	static bool isKernelRom(const char *filename);

	//! Check integrity of Character ROM image 
	/*! Returns true, iff the specified file contains a valid Character ROM image.
		File integrity is checked via the checkFileHeader function.
		@param filename Name of the file being loaded
	*/
	static bool isCharRom(const char *filename);

	//! Check integrity of ROM image 
	/*! Returns true, iff the specified file is one of the three possible ROM images.
		@param filename Name of the file being loaded
	*/
	static bool isRom(const char *filename);
	
	//! Callback function
	/*! This function is called by the CPU when the port lines change.
	 Besides others, the processor port register determines whether the RAM, ROM, or the IO space is visible. 
	 */ 
	 void processorPortHasChanged(uint8_t newPortLines);
		
public:
	
	//! Constructor
	C64Memory();
	
	//! Destructor
	~C64Memory();
	
	//! Restore initial state
	void reset();

	//! Load internal state from a file
	bool load(FILE *file);
	
	//! Save internal state to a file
	bool save(FILE *file);
	
	//! Dump current state into logfile
	void dumpState();

	//! Bind the virtual memory to the specified VIC chip.
	/*! The binding is irreversible and the function "call once". */
	void setVIC(VIC *v) { assert(vic == NULL); vic = v; }

	//! Bind the virtual memory to the specified SID chip.
	/*! The binding is irreversible and the function "call once". */
	void setSID(SID *s) { assert(sid == NULL); sid = s; }
	
	//! Bind the virtual memory to the specified CIA chip (CIA 1).
	/*! The binding is irreversible and the function "call once". */
	void setCIA1(CIA1 *c) { assert(cia1 == NULL); cia1 = c; }

	//! Bind the virtual memory to the specified CIA chip (CIA 2).
	/*! The binding is irreversible and the function "call once". */
	void setCIA2(CIA2 *c) { assert(cia2 == NULL); cia2 = c; }
		
	//! Return physical start address of RAM
	uint8_t *getRam() { return ram; }

	//! Return physical start address of ROM
	uint8_t *getRom() { return rom; }

	//! Load basic ROM image into memory 
	bool loadBasicRom(const char *filename);
	//! Load character ROM image into memory 
	bool loadCharRom(const char *filename);
	//! Load kernel ROM image into memory 
	bool loadKernelRom(const char *filename);
	
	//! Returns true, iff the Basic ROM is alrady loaded
	bool basicRomIsLoaded() { return basicRomFile != NULL; }
	//! Returns true, iff the Kernel ROM is alrady loaded
	bool kernelRomIsLoaded() { return kernelRomFile != NULL; }
	//! Returns true, iff the Character ROM is alrady loaded
	bool charRomIsLoaded() { return charRomFile != NULL; }

	//! Returns true, iff the provided address is in the Basic ROM address range
	static inline bool isBasicRomAddr(uint16_t addr) 
		{ return (0xA000 <= addr && addr <= 0xBFFF); }
	//! Returns true, iff the provided address is in the Character ROM address range
	static inline bool isCharRomAddr(uint16_t addr) 
		{ return (0xD000 <= addr && addr <= 0xDFFF); }
	//! Returns true, iff the provided address is in the Kernel ROM address range
	static inline bool isKernelRomAddr(uint16_t addr) 
		{ return (0xE000 <= addr); }
	//! Returns true, iff the provided address is in one of the three ROM address ranges
	static inline bool isRomAddr(uint16_t addr) 
		{ return isCharRomAddr(addr) || isKernelRomAddr(addr) || isBasicRomAddr(addr); }
				
	//! Read a BYTE from Color RAM.
	/*! The BYTE is always read from the color RAM, regardless of the value of the processor port register.
		\param offset Memory address relative to the beginning of the color Ram
		\return Color RAM contents at address addr
		\see peek 
	*/
	inline uint8_t peekColorRam(uint16_t offset) { assert(offset < 1024); return colorRam[offset]; } 
	
	bool isValidAddr(uint16_t addr, MemoryType type);
	uint8_t peekRam(uint16_t addr);
	uint8_t peekRom(uint16_t addr);
	uint8_t peekIO(uint16_t addr);
	uint8_t peekAuto(uint16_t addr);             
	                  																
	void pokeRam(uint16_t addr, uint8_t value);                  
	void pokeRom(uint16_t addr, uint8_t value);             
	void pokeIO(uint16_t addr, uint8_t value);
	void pokeAuto(uint16_t addr, uint8_t value);
};

#endif
