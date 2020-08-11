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

#include "C64Memory.h"

class C64Memory : public C64Component {

    // Current configuration
    MemConfig config;
    
    // Result of the latest inspection
    MemInfo info;
    
public:
    
    /* C64 bank mapping
     *
     *     BankMap[index][range] where
     *
     *             index = (EXROM, GAME, CHAREN, HIRAM, LORAM)
     *             range = upper four bits of address
     */
    MemoryType bankMap[32][16];
        
	// Random Access Memory
	u8 ram[65536];

    /* Color RAM
     * The color RAM is located in the I/O space, starting at $D800 and ending
     * at $DBFF. Only the lower four bits are accessible, the upper four bits
     * are open and can show any value.
     */
    u8 colorRam[1024];

    // Read Only Memory
	/* Only specific memory cells are valid ROM locations. In total, the C64
     * has three ROMs that are located at different addresses. Note, that the
     * ROMs do not span over the whole 64KB range. Therefore, only some
     * addresses are valid ROM addresses.
     */
    u8 rom[65536];
        
    // Peek source lookup table
    MemoryType peekSrc[16];
    
    // Poke target lookup table
    MemoryType pokeTarget[16];
    
    // Indicates if watchpoints should be checked
    bool checkWatchpoints = false;
    
    
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
    // Analyzing
    //
    
public:
    
    // Returns the result of the latest inspection
    MemInfo getInfo() { return HardwareComponent::getInfo(info); }
        
    //
    // Methods from HardwareComponent
    //
    
private:
    
	void _reset() override;
    void _inspect() override;
	void _dump() override;

public:
    
    // Erases the RAM with the provided init pattern
    void eraseWithPattern(RamPattern pattern);
    
    /* Updates the peek and poke lookup tables. The lookup values depend on
     * three processor port bits and the cartridge exrom and game lines.
     */
    void updatePeekPokeLookupTables();

    // Returns the current peek source of the specified memory address
    MemoryType getPeekSource(u16 addr) { return peekSrc[addr >> 12]; }
    
    // Returns the current poke target of the specified memory address
    MemoryType getPokeTarget(u16 addr) { return pokeTarget[addr >> 12]; }

    // Reads a value from memory
    u8 peek(u16 addr, MemoryType source);
    u8 peek(u16 addr, bool gameLine, bool exromLine);
    u8 peek(u16 addr) { return peek(addr, peekSrc[addr >> 12]); }
    u8 peekZP(u8 addr);
    u8 peekStack(u8 sp);
    u8 peekIO(u16 addr);

    // Reads a value from memory and discards the result (idle access)
    void peekIdle(u16 addr) { (void)peek(addr); }
    void peekZPIdle(u8 addr) { (void)peekZP(addr); }
    void peekStackIdle(u8 sp) { (void)peekStack(sp); }
    void peekIOIdle(u16 addr) { (void)peekIO(addr); }
    
    // Reads a value from memory without side effects
    u8 spypeek(u16 addr, MemoryType source);
    u8 spypeek(u16 addr) { return spypeek(addr, peekSrc[addr >> 12]); }
    u8 spypeekIO(u16 addr);
    u8 spypeekColor(u16 addr);

    // Writing a value into memory
    void poke(u16 addr, u8 value, MemoryType target);
    void poke(u16 addr, u8 value, bool gameLine, bool exromLine);
    void poke(u16 addr, u8 value) { poke(addr, value, pokeTarget[addr >> 12]); }
    void pokeZP(u8 addr, u8 value);
    void pokeStack(u8 sp, u8 value);
    void pokeIO(u16 addr, u8 value);
    
    // Reads a vector address from memory
    u16 nmiVector();
    u16 irqVector();
    u16 resetVector();
    
    // Returns a string representations for a portion of memory
    char *memdump(u16 addr, long num, bool hex, MemoryType src);
    char *hexdump(u16 addr, long num, MemoryType src) { return memdump(addr, num, true, src); }
    char *decdump(u16 addr, long num, MemoryType src) { return memdump(addr, num, false, src); }
    char *txtdump(u16 addr, long num, MemoryType src);

    char *memdump(u16 addr, long num, bool hex) { return memdump(addr, num, hex, peekSrc[addr >> 12]); }
    char *hexdump(u16 addr, long num) { return hexdump(addr, num, peekSrc[addr >> 12]); }
    char *decdump(u16 addr, long num) { return decdump(addr, num, peekSrc[addr >> 12]); }
    char *txtdump(u16 addr, long num) { return txtdump(addr, num, peekSrc[addr >> 12]); }
};

#endif
