// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"
#include "CartridgeTypes.h"

/* This class implements a Flash Rom module of type Am29F040B. Flash Roms
 * of this type are used, e.g., by the EasyFlash cartridge. The implementation
 * is based on the following ressources:
 *
 *       29F040.pdf : Data sheet published by AMD
 *   flash040core.c : Part of the VICE emulator
 */
class FlashRom : public C64Component {
            
    // Number of sectors in this Flash Rom
    static const usize numSectors = 8;
    
    // Size of a single sector in bytes (64 KB)
    static const usize sectorSize = 0x10000;
    
    // Total size of the Flash Rom in bytes (512 KB)
    static const usize romSize = 0x80000;

    // Current Flash Rom state
    FlashState state;

    // State taken after an operations has been completed
    FlashState baseState;
    
    // Flash Rom data
    u8 *rom = nullptr;
    
    
    //
    // Class methods
    //

public:

    // Checks whether the provided number is a valid bank number
    static bool isBankNumber(unsigned bank) { return bank < 64; }
    
    // Converts a Flash Rom state to a string
    static const char *getStateAsString(FlashState state);
    
    
    //
    // Constructing and serializing
    //
    
public:
    
    FlashRom(C64 &ref);
    ~FlashRom();
    const char *getDescription() const override { return "FlashRom"; }

    /* Loads an 8 KB chunk of Rom data from a buffer. This method is used when
     * loading the contents from a CRT file.
     */
    void loadBank(unsigned bank, u8 *data);
    
private:
    
    void _reset(bool hard) override;

    
    //
    // Analyzing
    //
    
private:
    
    void _dump(dump::Category category, std::ostream& os) const override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        << state
        << baseState;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }
    
    usize __size() { COMPUTE_SNAPSHOT_SIZE }
    isize _size() override { return __size() + romSize; }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    isize didLoadFromBuffer(const u8 *buffer) override;
    isize didSaveToBuffer(u8 *buffer) override;

    
    //
    // Accessing memory
    //
    
public:
    
    u8 peek(u32 addr);
    u8 peek(unsigned bank, u16 addr) {
        assert(isBankNumber(bank)); return peek(bank * 0x2000 + addr); }
    
    u8 spypeek(u32 addr) const;
    u8 spypeek(unsigned bank, u16 addr) const {
        assert(isBankNumber(bank)); return spypeek(bank * 0x2000 + addr); }
    
    void poke(u32 addr, u8 value);
    void poke(unsigned bank, u16 addr, u8 value) {
        assert(isBankNumber(bank)); poke(bank * 0x2000 + addr, value); }
    
    
    //
    // Performing flash operations
    //
    
    // Checks if addr serves as the first command address
    bool firstCommandAddr(u32 addr) { return (addr & 0x7FF) == 0x555; }

    // Checks if addr serves as the second command address
    bool secondCommandAddr(u32 addr) { return (addr & 0x7FF) == 0x2AA; }

    // Performs a "Byte Program" operation
    bool doByteProgram(u32 addr, u8 value);
    
    // Convenience wrapper with bank,offset addressing
    bool doByteProgram(unsigned bank, u16 addr, u8 value) {
        assert(isBankNumber(bank)); return doByteProgram(bank * 0x2000 + addr, value); }
    
    // Performs a "Sector Erase" operation
    void doSectorErase(u32 addr);
    
    // Convenience wrapper with bank,offset addressing
    void doSectorErase(unsigned bank, u16 addr) {
        assert(isBankNumber(bank)); doSectorErase(bank * 0x2000 + addr); }
    
    // Performs a "Chip Erase" operation
    void doChipErase();
};
