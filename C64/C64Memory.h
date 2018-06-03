/*!
 * @header      C64Memory.h
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

#ifndef _C64MEMORY_INC
#define _C64MEMORY_INC

#include "Memory.h"

/*! @brief    This class represents RAM and ROM of the virtual C64
 *  @details  Due to the limited address space, RAM, ROM, and I/O memory are superposed,
 *            wich means that they share the same memory locations. The currently visible
 *            memory is determined by the contents of the processor port (memory location 1)
 *            and the current values of the Exrom and Game line.
 */
class C64Memory : public Memory {

    //! @brief    C64 bank mapping
    //
    // If x = (EXROM, GAME, CHAREN, HIRAM, LORAM), then
    //   BankMap[x][0] = mapping for range $1000 - $7FFF
    //   BankMap[x][1] = mapping for range $8000 - $9FFF
    //   BankMap[x][2] = mapping for range $A000 - $BFFF
    //   BankMap[x][3] = mapping for range $C000 - $CFFF
    //   BankMap[x][4] = mapping for range $D000 - $DFFF
    //   BankMap[x][5] = mapping for range $E000 - $FFFF

    const MemoryType BankMap[32][6] = {
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_CRTHI, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_CRTLO, M_CRTHI, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_CRTHI, M_RAM,  M_IO,   M_KERNAL},
        {M_RAM,  M_CRTLO, M_CRTHI, M_RAM,  M_IO,   M_KERNAL},
        
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_CRTLO, M_BASIC, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_KERNAL},
        {M_RAM,  M_CRTLO, M_BASIC, M_RAM,  M_IO,   M_KERNAL},
        
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_BASIC, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_KERNAL},
        {M_RAM,  M_RAM,   M_BASIC, M_RAM,  M_IO,   M_KERNAL}
    };

public:		
			
	//! @brief    Random Access Memory
	uint8_t ram[65536];

    /*! @brief    Color RAM
     *  @details  The color RAM is located in the I/O space, starting at $D800 and
     *            ending at $DBFF. Only the lower four bits are accessible, the upper
     *            four bits are open and can show any value.
     */
    uint8_t colorRam[1024];

    //! @brief    Read Only Memory
	/*! @details  Only specific memory cells are valid ROM locations. In total, the C64
     *            has three ROMs that are located at different addresses.
     *            Note, that the ROMs do not span over the whole 64KB range. Therefore,
     *            only some addresses are valid ROM addresses.
     */
    uint8_t rom[65536];
    
    /*! @brief    Checks the integrity of a Basic ROM image.
     *  @details  Returns true, iff the specified file contains a valid Basic ROM image.
     */
    static bool isBasicRom(const char *filename);
    
    /*! @brief    Checks the integrity of a Kernal ROM image file.
     *  @details  Returns true, iff the specified file contains a valid Kernal ROM image.
     */
    static bool isKernalRom(const char *filename);
    
    /*! @brief    Checks the integrity of a Character ROM image file.
     *  @details  Returns true, iff the specified file contains a valid Character ROM image.
     */
    static bool isCharRom(const char *filename);
    
    /*! @brief    Checks the integrity of a ROM image file
     *  @details  Returns true, iff the specified file is one of the three possible ROM images.
     */
    static bool isRom(const char *filename);

private:
	
	/*! @brief    File name of the Character ROM image.
	 *  @details  The file name is set in loadRom().
     */
	char *charRomFile;

    /*! @brief    File name of the Kernal ROM image.
     *  @details  The file name is set in loadRom().
     */
	char *kernalRomFile;

    /*! @brief    File name of the Basic ROM image.
     *  @details  The file name is set in loadRom().
     */
	char *basicRomFile;

			
public:
	
	//! @brief    Constructor
	C64Memory();
	
	//! @brief    Destructor
	~C64Memory();
	
	//! @brief    Method from VirtualComponent
	void reset();
    
    //! @brief    Restors initial state, but keeps RAM alive
    void resetWithoutRAM();

	//! @brief    Method from VirtualComponent
	void dumpState();
		
	//! @brief    Flashes a Basic ROM image into memory
	bool loadBasicRom(const char *filename);
    
	//! @brief    Flashes a Character ROM image into memory
	bool loadCharRom(const char *filename);
    
	//! @brief    Flashes a Kernal ROM image into memory
	bool loadKernalRom(const char *filename);

	//! @brief    Returns true, iff the Basic ROM is alrady loaded
	bool basicRomIsLoaded() { return basicRomFile != NULL; }
    
	//! @brief    Returns true, iff the Kernal ROM is alrady loaded
	bool kernalRomIsLoaded() { return kernalRomFile != NULL; }
    
	//! @brief    Returns true, iff the Character ROM is alrady loaded
	bool charRomIsLoaded() { return charRomFile != NULL; }

    
private:
    
    //! @brief    Lookup table for peek()
    MemoryType peekSrc[16];
    
    //! @brief    Lookup table for poke()
    MemoryType pokeTarget[16];
    
    
public:
    
    /*! @brief    Updates the peek and poke lookup tables.
     *  @details  The lookup values depend on three processor port bits
     *            and the cartridge exrom and game lines.
     */
    void updatePeekPokeLookupTables();
    
    //! @brief    Returns the current peek source of the specified memory address
    MemoryType getPeekSource(uint16_t addr) { return peekSrc[addr >> 12]; }
    
    //! @brief    Returns the current poke target of the specified memory address
    MemoryType getPokeTarget(uint16_t addr) { return pokeTarget[addr >> 12]; }

    // Reading from memory
    uint8_t peek(uint16_t addr, MemoryType source);
    uint8_t peek(uint16_t addr) { return peek(addr, peekSrc[addr >> 12]); }
    uint8_t peekIO(uint16_t addr);
    
    // Reading from memory without side effects
    uint8_t snoop(uint16_t addr, MemoryType source);
    uint8_t snoop(uint16_t addr) { return snoop(addr, peekSrc[addr >> 12]); }
    uint8_t snoopIO(uint16_t addr);
    
    // Writing into memory
    void poke(uint16_t addr, uint8_t value, MemoryType target);
    void poke(uint16_t addr, uint8_t value) { poke(addr, value, pokeTarget[addr >> 12]); }
    void pokeIO(uint16_t addr, uint8_t value);
};

#endif
