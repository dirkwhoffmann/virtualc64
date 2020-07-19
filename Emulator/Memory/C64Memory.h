// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _C64MEMORY_H
#define _C64MEMORY_H

#include "Memory.h"

/*! @brief    This class represents RAM and ROM of the virtual C64
 *  @details  Due to the limited address space, RAM, ROM, and I/O memory are
 *            superposed, wich means that they share the same memory locations.
 *            The currently visible memory is determined by the contents of the
 *            processor port (memory address 1) and the current values of the
 *            Exrom and Game line.
 */
class C64Memory : public Memory {

    // Current configuration
    MemConfig config;
    
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
	u8 ram[65536];

    /*! @brief    Color RAM
     *  @details  The color RAM is located in the I/O space, starting at $D800 and
     *            ending at $DBFF. Only the lower four bits are accessible, the upper
     *            four bits are open and can show any value.
     */
    u8 colorRam[1024];

    //! @brief    Read Only Memory
	/*! @details  Only specific memory cells are valid ROM locations. In total, the C64
     *            has three ROMs that are located at different addresses.
     *            Note, that the ROMs do not span over the whole 64KB range. Therefore,
     *            only some addresses are valid ROM addresses.
     */
    u8 rom[65536];
        
    //! @brief    Peek source lookup table
    MemoryType peekSrc[16];
    
    //! @brief    Poke target lookup table
    MemoryType pokeTarget[16];
    
    
    //
    // Constructing and serializing
    //
    
public:
    
	C64Memory(C64 &ref);
    
    //
    // Configuring
    //
    
    MemConfig getConfig() { return config; }
    
    RamPattern getRamPattern() { return config.ramPattern; }
    void setRamPattern(RamPattern pattern) { config.ramPattern = pattern; }

        
    //
    // Methods from HardwareComponent
    //
    
public:
    
	void _reset() override;
    
private:
    
	void _dump() override;

    
public:
    
    //! @brief    Erases the memory with the provided init pattern
    void eraseWithPattern(RamPattern pattern);
    
    /*! @brief    Updates the peek and poke lookup tables.
     *  @details  The lookup values depend on three processor port bits
     *            and the cartridge exrom and game lines.
     */
    void updatePeekPokeLookupTables();

    //! @brief    Returns the current peek source of the specified memory address
    MemoryType getPeekSource(u16 addr) { return peekSrc[addr >> 12]; }
    
    //! @brief    Returns the current poke target of the specified memory address
    MemoryType getPokeTarget(u16 addr) { return pokeTarget[addr >> 12]; }

    // Reading from memory
    u8 peek(u16 addr, MemoryType source);
    u8 peek(u16 addr, bool gameLine, bool exromLine);
    u8 peek(u16 addr) override { return peek(addr, peekSrc[addr >> 12]); }
    u8 peekZP(u8 addr) override;
    u8 peekIO(u16 addr);
    
    // Reading from memory without side effects
    u8 spypeek(u16 addr, MemoryType source);
    u8 spypeek(u16 addr) override { return spypeek(addr, peekSrc[addr >> 12]); }
    u8 spypeekIO(u16 addr);
    
    // Writing into memory
    void poke(u16 addr, u8 value, MemoryType target);
    void poke(u16 addr, u8 value, bool gameLine, bool exromLine);
    void poke(u16 addr, u8 value) override { poke(addr, value, pokeTarget[addr >> 12]); }
    void pokeZP(u8 addr, u8 value) override;
    void pokeIO(u16 addr, u8 value);
    
    //! @brief    Reads the NMI vector from memory.
    u16 nmiVector();
    
    //! @brief    Reads the IRQ vector from memory.
    u16 irqVector();
    
    //! @brief    Reads the Reset vector from memory.
    u16 resetVector();
};

#endif
