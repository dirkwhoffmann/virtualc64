/*
 * (C) 2008 - 2015 Dirk W. Hoffmann. All rights reserved.
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
class SIDWrapper;

//! This class represents the RAM and ROM of a virtual C64
/*! Note that the RAM, ROM, and the I/O space are superposed and therefore share the same locations in memory.
    The contents of memory location 0x0001 determines which kind of memory is currently visible. */
class C64Memory : public Memory {

    // Memory source identifiers used inside the peek/poke lookup tables
    enum MemorySource
    {
        M_RAM = 1,
        M_ROM,
        M_IO,
        M_CRTLO,
        M_CRTHI,
        M_PP,
        M_NONE
    };

#define M_CHAR M_ROM
#define M_KERNEL M_ROM
#define M_BASIC M_ROM

    //! C64 bank mapping
    //
    // If x = (EXROM, GAME, CHAREN, HIRAM, LORAM), then
    //   BankMap[x][0] = mapping for range $1000 - $7FFF
    //   BankMap[x][1] = mapping for range $8000 - $9FFF
    //   BankMap[x][2] = mapping for range $A000 - $BFFF
    //   BankMap[x][3] = mapping for range $C000 - $CFFF
    //   BankMap[x][4] = mapping for range $D000 - $DFFF
    //   BankMap[x][5] = mapping for range $E000 - $FFFF

    const MemorySource BankMap[32][6] = {
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_CRTHI, M_RAM,  M_CHAR, M_KERNEL},
        {M_RAM,  M_CRTLO, M_CRTHI, M_RAM,  M_CHAR, M_KERNEL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_CRTHI, M_RAM,  M_IO,   M_KERNEL},
        {M_RAM,  M_CRTLO, M_CRTHI, M_RAM,  M_IO,   M_KERNEL},
        
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_KERNEL},
        {M_RAM,  M_CRTLO, M_BASIC, M_RAM,  M_CHAR, M_KERNEL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_KERNEL},
        {M_RAM,  M_CRTLO, M_BASIC, M_RAM,  M_IO,   M_KERNEL},
        
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
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_KERNEL},
        {M_RAM,  M_RAM,   M_BASIC, M_RAM,  M_CHAR, M_KERNEL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_KERNEL},
        {M_RAM,  M_RAM,   M_BASIC, M_RAM,  M_IO,   M_KERNEL}
    };

public:		

	//! Reference to the connected VIC chip 
	// VIC *vic;
	
	//! Reference to the connected SID chip
	SIDWrapper *sid; 
			
	//! Virtual RAM
	uint8_t ram[65536];

    //! Virtual color RAM
    /*! The color RAM is located in the I/O space, starting at D800 and ending at DBFF
     Only the lower four bits are accessible, the upper four bits are open and can show any value */
    uint8_t colorRam[1024];

    //! Virtual ROM
	/*! Only specific memory cells are valid ROM locations. In total, the C64 has three ROMs that
		are located at different addresses in the ROM space. Note, that the ROMs do not span over
		the whole 64k range. Therefore, only some address are valid ROM addresses.
		\see isRomAddr */
	uint8_t rom[65536];
    
public:
    
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

    //! Returns true, iff the provided address is in the Basic ROM address range
    static inline bool isBasicRomAddr(uint16_t addr) { return (0xA000 <= addr && addr <= 0xBFFF); }
    
    //! Returns true, iff the provided address is in the Character ROM address range
    static inline bool isCharRomAddr(uint16_t addr) { return (0xD000 <= addr && addr <= 0xDFFF); }
    
    //! Returns true, iff the provided address is in the Kernel ROM address range
    static inline bool isKernelRomAddr(uint16_t addr) { return (0xE000 <= addr); }
    
    //! Returns true, iff the provided address is in the possible cartridge address ranges
    static inline bool isCartridgeRomAddr(uint16_t addr)
    { return (0x8000 <= addr && addr <= 0x9FFF)||(0xA000 <= addr && addr <= 0xBFFF)||(0xE000 <= addr && addr <= 0xFFFE); }
    
    //! Returns true, iff the provided address is in one of the three ROM address ranges
    static inline bool isRomAddr(uint16_t addr)
    { return isCharRomAddr(addr) || isKernelRomAddr(addr) || isBasicRomAddr(addr) || isCartridgeRomAddr(addr); }

    
private:
	
	//! File name of the Character ROM image.
	/*! The file name is set by the loadRom routine. It is saved for further reference, so the ROM can be reloaded any tim e. */
	char *charRomFile;

	//! File name of the Kernel ROM image.
	/*! The file name is set by the loadRom routine. It is saved for further reference, so the ROM can be reloaded any tim e. */
	char *kernelRomFile;

	//! File name of the Basic ROM image.
	/*! The file name is set by the loadRom routine. It is saved for further reference, so the ROM can be reloaded any tim e. */
	char *basicRomFile;	

			
public:
	
	//! Constructor
	C64Memory();
	
	//! Destructor
	~C64Memory();
	
	//! Restore initial state
	void reset();
	
	//! Dump current state into logfile
	void dumpState();
		
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


private:
    
    //! Lookup table for peek function
    MemorySource peekSrc[16];
    
    //! Lookup table for poke function
    MemorySource pokeTarget[16];
    
public:
    
    //! Updates peekSrc and pokeTarget
    /*! Values depend on three processor port bits and the cartridge exrom and game lines */
    void updatePeekPokeLookupTables();

    //! Returns true iff the provided address is a valid address of the specified type */
	bool isValidAddr(uint16_t addr, MemoryType type);

    //! Read value from RAM
	uint8_t peekRam(uint16_t addr); 

    //! Read value from ROM
    uint8_t peekRom(uint16_t addr);

    //! Read value from IO space
    uint8_t peekIO(uint16_t addr);

    //! Read value from the currently visible source (RAM, ROM, I/O etc.)
    uint8_t peek(uint16_t addr);

    //! Write value to RAM
	void pokeRam(uint16_t addr, uint8_t value);                  

    //! Write value to ROM
    void pokeRom(uint16_t addr, uint8_t value);

    //! Write value to I/O space
    void pokeIO(uint16_t addr, uint8_t value);

    //! Write value to the currently visible source (RAM, I/O etc.)    
    void poke(uint16_t addr, uint8_t value);
};

#endif
