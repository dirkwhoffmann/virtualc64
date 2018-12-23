/*!
 * @header      C64Memory.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
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
 *  @details  Due to the limited address space, RAM, ROM, and I/O memory are
 *            superposed, wich means that they share the same memory locations.
 *            The currently visible memory is determined by the contents of the
 *            processor port (memory address 1) and the current values of the
 *            Exrom and Game line.
 */
class C64Memory : public Memory {

public:
    
    /*! @brief    C64 bank mapping
     *  @details  BankMap[index][range] where
     *             index = (EXROM, GAME, CHAREN, HIRAM, LORAM)
     *             range = upper four bits of address
     */
    MemoryType bankMap[32][16];
    
    /*
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
    */
    
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
    
    //! @brief    RAM init pattern type
    RamInitPattern ramInitPattern;
    
    //! @brief    Peek source lookup table
    MemoryType peekSrc[16];
    
    //! @brief    Poke target lookup table
    MemoryType pokeTarget[16];
    
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
	void dump();

	//! @brief    Returns true, iff the Basic ROM has been loaded
	bool basicRomIsLoaded() { return rom[0xA000] != 0x00; }
    
    //! @brief    Deletes the Basic ROM from memory
    void deleteBasicRom() { memset(rom + 0xA000, 0, 0x2000); }
    
    //! @brief    Returns true, iff the Character ROM has been loaded
    bool characterRomIsLoaded() { return rom[0xD000] != 0x00; }

    //! @brief    Deletes the Character ROM from memory
    void deleteCharacterRom() { memset(rom + 0xD000, 0, 0x1000); }

    //! @brief    Returns true, iff the Kernal ROM has been loaded
	bool kernalRomIsLoaded() { return rom[0xE000] != 0x00; }

    //! @brief    Deletes the Kernal ROM from memory
    void deleteKernalRom() { memset(rom + 0xE000, 0, 0x2000); }

    /*! @brief    Computes a 64-bit fingerprint for the Basic ROM.
     *  @return   fingerprint or 0, if no Basic ROM is installed.
     */
    uint64_t basicRomFingerprint() {
        return basicRomIsLoaded() ? fnv_1a(rom + 0xA000, 0x2000) : 0; }

    /*! @brief    Computes a 64-bit fingerprint for the Character ROM.
     *  @return   fingerprint or 0, if no Basic ROM is installed.
     */
    uint64_t characterRomFingerprint() {
        return characterRomIsLoaded() ? fnv_1a(rom + 0xD000, 0x1000) : 0; }

    /*! @brief    Computes a 64-bit fingerprint for the Kernal ROM.
     *  @return   fingerprint or 0, if no Basic ROM is installed.
     */
    uint64_t kernalRomFingerprint() {
        return kernalRomIsLoaded() ? fnv_1a(rom + 0xE000, 0x2000) : 0; }
    
public:
    
    //! @brief    Returns the currently used RAM init pattern.
    RamInitPattern getRamInitPattern() { return ramInitPattern; }
    
    //! @brief    Sets the RAM init pattern type.
    void setRamInitPattern(RamInitPattern pattern) { ramInitPattern = pattern; }

    //! @brief    Erases the memory with the provided init pattern
    void eraseWithPattern(RamInitPattern pattern);
    
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
    uint8_t peek(uint16_t addr, bool gameLine, bool exromLine);
    uint8_t peek(uint16_t addr) { return peek(addr, peekSrc[addr >> 12]); }
    uint8_t peekZP(uint8_t addr);
    uint8_t peekIO(uint16_t addr);
    
    // Reading from memory without side effects
    uint8_t spypeek(uint16_t addr, MemoryType source);
    uint8_t spypeek(uint16_t addr) { return spypeek(addr, peekSrc[addr >> 12]); }
    uint8_t spypeekIO(uint16_t addr);
    
    // Writing into memory
    void poke(uint16_t addr, uint8_t value, MemoryType target);
    void poke(uint16_t addr, uint8_t value, bool gameLine, bool exromLine);
    void poke(uint16_t addr, uint8_t value) { poke(addr, value, pokeTarget[addr >> 12]); }
    void pokeZP(uint8_t addr, uint8_t value);
    void pokeIO(uint16_t addr, uint8_t value);
};

#endif
