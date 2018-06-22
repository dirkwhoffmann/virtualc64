/*!
 * @header      VC151Memory.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2008 - 2016 Dirk W. Hoffmann
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
	IEC *iec;

	//! @brief    Reference to the connected disk drive
	VC1541 *floppy;
	
    //! @brief    Random Access Memory
    uint8_t ram[0x0800];
    
    //! @brief    Read Only Memory
    uint8_t rom[0x4000];
    
    /*! @brief    File name of the VC1541 ROM image.
     *  @details  The file name is set in loadRom().
     */
	char *romFile;

    /*! @brief    Checks the integrity of a VC1541 ROM image.
     *  @details  Returns true, iff the specified file contains a valid
     *            VC1541 ROM image.
     *            File integrity is checked via the checkFileHeader function.
     */
	static bool is1541Rom(const char *filename);
	
public:
	
	//! @brief    Constructor
	VC1541Memory();
	
	//! @brief    Destructor
	~VC1541Memory();

	//! @brief    Restores the initial state.
	void reset();
		
	//! @brief    Prints debugging information
	void dumpState();
		
	/*! @brief    Loads a ROM image into memory.
	 *  @details  The function automatically determines the type of the
     *            specified file. In case of a valid ROM image,
     *            it is loaded into the ROM space at the proper location.
     *  @return   Returns true, if the file could be loaded successfully.
     *            In case of an error (file not found, the file is no proper
     *            ROM image, ...) the function returns false.
     */
	bool loadRom(const char *filename);

	//! @brief    Returns true, iff the ROM image is alrady loaded
	bool romIsLoaded() { return romFile != NULL; }
				
    // Reading from memory
    uint8_t peek(uint16_t addr, MemoryType source);
    uint8_t peek(uint16_t addr);
    
    // Snooping in memory (no side effects)
    uint8_t snoop(uint16_t addr, MemoryType source);
    uint8_t snoop(uint16_t addr);
    
    // Writing into memory
    void poke(uint16_t addr, uint8_t value, MemoryType target);
    void poke(uint16_t addr, uint8_t value);
    void pokeIO(uint16_t addr, uint8_t value);
};

#endif
