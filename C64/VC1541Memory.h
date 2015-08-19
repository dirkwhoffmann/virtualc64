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

#ifndef _VC1541MEMORY_INC
#define _VC1541MEMORY_INC

#include "Memory.h"

class VC1541;

//! This class represents the RAM and ROM of a virtual VC1541 floopy disk drive
class VC1541Memory : public Memory {

private: 
	//! Reference to the connected IEC bus
	IEC *iec;

	//! Reference to the connected disk drive
	VC1541 *floppy;
	
public:		
	//! Virtual memory storage
	uint8_t mem[65536];
	
	//! File name of the ROM image.
	/*! The file name is set by the loadRom routine. It is saved for further reference, so the ROM can be reloaded any time. */
	char *romFile;

	//! Check integrity of ROM image 
	/*! Returns true, iff the specified file contains a valid VC1541 ROM image.
	    File integrity is checked via the checkFileHeader function. 
	    \param filename Name of the file being loaded
		\see C64:loadRom
	*/
	static bool is1541Rom(const char *filename);
	
public:
	
	//! Constructor
	VC1541Memory();
	
	//! Destructor
	~VC1541Memory();

	//! Restore initial state
	void reset(C64 *c64);
	
    //! Size of internal state
    uint32_t stateSize();

	//! Load state
	void loadFromBuffer(uint8_t **buffer);
	
	//! Save state
	void saveToBuffer(uint8_t **buffer);	
	
	//! Dump current state into logfile
	void dumpState();
		
	//! Load a ROM image into memory.
	/*! The function automatically determines the type of the specified file. In case of a valid ROM image, it
		is loaded into the ROM space at the proper location. 
		\param filename Filename of the ROM Image
		\return Returns true, if the file could be loaded successfully. In case of an error (file not found, 
		the file is no proper ROM image, ...) the function returns false.
	*/
	bool loadRom(const char *filename);

	//! Returns true, iff the ROM image is alrady loaded
	bool romIsLoaded() { return romFile != NULL; }
				
	// Virtual fuctions that need to be implemented...
	bool isValidAddr(uint16_t addr, MemoryType type);
	uint8_t peekRam(uint16_t addr);
	uint8_t peekRom(uint16_t addr);
	uint8_t peekIO(uint16_t addr);
	uint8_t peek(uint16_t addr);             
	                  																
	void pokeRam(uint16_t addr, uint8_t value);                  
	void pokeRom(uint16_t addr, uint8_t value);             
	void pokeIO(uint16_t addr, uint8_t value);
	void poke(uint16_t addr, uint8_t value);
};

#endif
