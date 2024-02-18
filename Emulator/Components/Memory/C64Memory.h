// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "MemoryTypes.h"
#include "SubComponent.h"

namespace vc64 {

class C64Memory : public SubComponent, public Inspectable<MemInfo, Void> {

    // Current configuration
    MemConfig config = { };
    
    // Result of the latest inspection
    mutable MemInfo info = { };
    
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
    // Initializing
    //
    
public:
    
    C64Memory(C64 &ref);
    
    
    //
    // Methods from CoreObject
    //
    
private:
    
    const char *getDescription() const override { return "C64Memory"; }
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from CoreComponent
    //

private:

    void _reset(bool hard) override;
    
    /*
    template <class T>
    void applyToRoms(T& worker)
    {
        worker

        << rom;
    }
    */

    template <class T>
    void serialize(T& worker)
    {
        if (util::isSoftResetter(worker)) return;

        worker

        << ram
        << colorRam;

        if (util::isResetter(worker)) return;

        worker

        << peekSrc
        << pokeTarget
        << config.saveRoms
        << config.ramPattern;

    }
    
    void operator << (util::SerChecker &worker) override { serialize(worker); }
    void operator << (util::SerCounter &worker) override;
    void operator << (util::SerResetter &worker) override { serialize(worker); }
    void operator << (util::SerReader &worker) override;
    void operator << (util::SerWriter &worker) override;


    //
    // Methods from Inspectable
    //

public:

    bool autoInspect() const override;
    void recordState(MemInfo &result) const override;


    //
    // Configuring
    //
    
public:
    
    static MemConfig getDefaultConfig();
    const MemConfig &getConfig() const { return config; }
    void resetConfig() override;

    i64 getConfigItem(Option option) const;
    void setConfigItem(Option option, i64 value);
    
    
    //
    // Analyzing
    //
    
public:
    
    // MemInfo getInfo() const { return CoreComponent::getInfo(info); }

private:
    
    // void _inspect() const override;
    
    
    //
    // Accessing
    //

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
    u8 spypeek(u16 addr, MemoryType source) const;
    u8 spypeek(u16 addr) const { return spypeek(addr, peekSrc[addr >> 12]); }
    u8 spypeekIO(u16 addr) const;
    u8 spypeekColor(u16 addr) const;

    // Writing a value into memory
    void poke(u16 addr, u8 value, MemoryType target);
    void poke(u16 addr, u8 value, bool gameLine, bool exromLine);
    void poke(u16 addr, u8 value) { poke(addr, value, pokeTarget[addr >> 12]); }
    void pokeZP(u8 addr, u8 value);
    void pokeStack(u8 sp, u8 value);
    void pokeIO(u16 addr, u8 value);
    
    // Reads a vector address from memory
    u16 nmiVector() const;
    u16 irqVector() const;
    u16 resetVector();
    
    // Returns a string representations for a portion of memory
    string memdump(u16 addr, isize num, bool hex, isize pads, MemoryType src) const;
    string hexdump(u16 addr, isize num, isize pads, MemoryType src) const;
    string decdump(u16 addr, isize num, isize pads, MemoryType src) const;
    string txtdump(u16 addr, isize num, MemoryType src) const;

    // Convenience wrappers
    string memdump(u16 addr, isize num, bool hex) { return memdump(addr, num, hex, 1, peekSrc[addr >> 12]); }
    string hexdump(u16 addr, isize num) { return hexdump(addr, num, 1, peekSrc[addr >> 12]); }
    string decdump(u16 addr, isize num) { return decdump(addr, num, 1, peekSrc[addr >> 12]); }
    string txtdump(u16 addr, isize num) { return txtdump(addr, num, peekSrc[addr >> 12]); }

    // Dumps a portion of memory to a stream
    void memDump(std::ostream& os, u16 addr, isize numLines = 16, bool hex = true);
};

}
