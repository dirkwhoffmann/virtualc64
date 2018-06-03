/*!
 * @header      C64Memory.h
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

#ifndef _C64MEMORY_INC
#define _C64MEMORY_INC

#include "Memory.h"

// Forward declarations
class VIC;
class SIDBridge;

/*! @brief    This class represents the RAM and ROM of a virtual C64
 *  @details  Note that the RAM, ROM, and the I/O space are superposed and therefore share the same locations 
 *            in memory. The contents of memory location 0x0001 determines which memory is currently visible. 
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
			
	//! @brief    The C64s Random Access Memory
	uint8_t ram[65536];

    /*! @brief    The C64s color RAM
     *  @details  The color RAM is located in the I/O space, starting at $D800 and ending at $DBFF
     *            Only the lower four bits are accessible, the upper four bits are open and can show any value.
     */
    uint8_t colorRam[1024];

    //! @brief    The C64s Read Only Memory
	/*! @details  Only specific memory cells are valid ROM locations. In total, the C64 has three ROMs that
     *            are located at different addresses in the ROM space. Note, that the ROMs do not span over
     *            the whole 64k range. Therefore, only some addresses are valid ROM addresses.
     */
    uint8_t rom[65536];
    
public:
    
    /*! @brief    Checks the integrity of a Basic ROM image.
     *  @details  Returns true, iff the specified file contains a valid Basic ROM image.
     *            File integrity is checked via the checkFileHeader function.
     */
    static bool isBasicRom(const char *filename);
    
    /*! @brief    Checks the integrity of a Kernal ROM image file.
     *  @details  Returns true, iff the specified file contains a valid Kernal ROM image.
     *            File integrity is checked via the checkFileHeader function.
     */
    static bool isKernalRom(const char *filename);
    
    /*! @brief    Checks the integrity of a Character ROM image file.
     *  @details  Returns true, iff the specified file contains a valid Character ROM image.
     *            File integrity is checked via the checkFileHeader function.
     */
    static bool isCharRom(const char *filename);
    
    /*! @brief    Checks the integrity of a ROM image file
     *  @details  Returns true, iff the specified file is one of the three possible ROM images.
     */
    static bool isRom(const char *filename);

private:
	
	/*! @brief    File name of the Character ROM image.
	 *  @details  The file name is set in loadRom(). It is saved for further reference, so the ROM can be reloaded
     *            any time. 
     */
	char *charRomFile;

    /*! @brief    File name of the Kernal ROM image.
     *  @details  The file name is set in loadRom(). It is saved for further reference, so the ROM can be reloaded
     *            any time.
     */
	char *kernalRomFile;

    /*! @brief    File name of the Basic ROM image.
     *  @details  The file name is set in loadRom(). It is saved for further reference, so the ROM can be reloaded
     *            any time.
     */
	char *basicRomFile;

			
public:
	
	//! @brief    Constructor
	C64Memory();
	
	//! @brief    Destructor
	~C64Memory();
	
	//! @brief    Restores initial state
	void reset();
    
    //! @brief    Restors initial state, but keeps RAM alive
    void resetWithoutRAM();

	//! @brief    Prints debug information
	void dumpState();
		
	//! @brief    Loads a basic ROM image into memory
	bool loadBasicRom(const char *filename);
    
	//! @brief    Loads a character ROM image into memory
	bool loadCharRom(const char *filename);
    
	//! @brief    Loads a kernal ROM image into memory
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
    
    //! @brief    Virtual fuctions from Memory class
    uint8_t peek(uint16_t addr, MemoryType source);
    uint8_t peek(uint16_t addr) { return peek(addr, peekSrc[addr >> 12]); }
    uint8_t snoop(uint16_t addr, MemoryType source);
    uint8_t snoop(uint16_t addr) { return snoop(addr, peekSrc[addr >> 12]); }

    //! @brief    Peek function for the I/O space (called inside peek)
    uint8_t peekIO(uint16_t addr);

    //! @brief    Snoop function for the I/O space (called inside snoop)
    uint8_t snoopIO(uint16_t addr);


    
    /*
    uint8_t spy(uint16_t addr, MemoryType src);
    uint8_t spyIO(uint16_t addr);
    uint8_t spy(uint16_t addr);
*/
    
    //! @brief    Write a byte into RAM.
    void pokeRam(uint16_t addr, uint8_t value) { ram[addr] = value; }

    //! @brief    Write a byte into ROM.
    void pokeRom(uint16_t addr, uint8_t value) { rom[addr] = value; }

    //! @brief    Returns the current poke target of the specified memory address
    MemoryType getPokeTarget(uint16_t addr) { return pokeTarget[addr >> 12]; }

    //! @brief    Writes a byte into memory.
    void pokeTo(uint16_t addr, uint8_t value, MemoryType target);

    //! @brief    Writes a byte into memory.
    //! @details  The memory target is read from the poke lookup table.
    void poke(uint16_t addr, uint8_t value);
    
    //! @brief    Write a byte into I/O space.
    void pokeIO(uint16_t addr, uint8_t value);

};

#endif
