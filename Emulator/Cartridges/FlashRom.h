// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _FLASHROM_H
#define _FLASHROM_H

#include "C64Component.h"

/*! @brief    This class implements a Flash Rom module of type Am29F040B
 *  @details  Flash Rom modules of this type are used, e.g., by the EasyFlash
 *            cartridge.
 *            The implementation is based on the following ressources:
 *            29F040.pdf:     Data sheet published by AMD
 *            flash040core.c: Part of the VICE emulator
 */
class FlashRom : public C64Component {
    
    private:
    
    //! @brief    Flash Rom states (taken from VICE)
    typedef enum {
        FLASH_READ = 0,
        FLASH_MAGIC_1,
        FLASH_MAGIC_2,
        FLASH_AUTOSELECT,
        FLASH_BYTE_PROGRAM,
        FLASH_BYTE_PROGRAM_ERROR,
        FLASH_ERASE_MAGIC_1,
        FLASH_ERASE_MAGIC_2,
        FLASH_ERASE_SELECT,
        FLASH_CHIP_ERASE,
        FLASH_SECTOR_ERASE,
        FLASH_SECTOR_ERASE_TIMEOUT,
        FLASH_SECTOR_ERASE_SUSPEND
    } FlashRomState;
    
    //! @brief    Current Flash Rom state
    FlashRomState state;

    //! @brief    State taken after an operations has been completed.
    FlashRomState baseState;

    //! @brief    Number of sectors in this Flash Rom
    size_t numSectors;
    
    //! @brief    Size of a single sector in bytes
    size_t sectorSize; // 64 KB
    
    //! @brief    Total size of the Flash Rom in bytes
    size_t size; // 512 KB
    
    //! @brief    Flash Rom data
    u8 *rom;
    
    public:
    
    //
    //! @brief    Class methods
    //
    
    //! @brief    Validity check for bank numbers
    static bool isBankNumber(unsigned bank) { return bank < 64; }
    
    //! @brief    Converts a Flash Rom state to a string
    static const char *getStateAsString(FlashRomState state);
    
    
    //
    // Constructing and serializing
    //
    
public:
    
    FlashRom(C64 &ref);
    ~FlashRom();
    
    /* Loads an 8 KB chunk of Rom data from a buffer. This method is used when
     * loading the contents from a CRT file.
     */
    void loadBank(unsigned bank, u8 *data);
    
    
    //
    // Methods from HardwareComponent
    //
    
public:
    
    void reset() override;

private:

    void _dump() override;
    
    
    //
    //! @functiongroup Accessing Rom cells
    //
    
public:
    
    //! @brief    Reads a Rom cell
    u8 peek(u32 addr);
    
    //! @brief    Convenience wrapper with bank,offset addressing
    u8 peek(unsigned bank, u16 addr) {
        assert(isBankNumber(bank)); return peek(bank * 0x2000 + addr); }
    
    //! @brief    Reads a Rom cell without side effects
    u8 spypeek(u32 addr) { return peek(addr); }
    
    //! @brief    Convenience wrapper with bank,offset addressing
    u8 spypeek(unsigned bank, u16 addr) {
        assert(isBankNumber(bank)); return peek(bank * 0x2000 + addr); }
    
    //! @brief    Writes a Rom cell
    void poke(u32 addr, u8 value);
    
    //! @brief    Convenience wrapper with bank,offset addressing
    void poke(unsigned bank, u16 addr, u8 value) {
        assert(isBankNumber(bank)); poke(bank * 0x2000 + addr, value); }
    
    
    //
    //! @functiongroup Performing flash operations
    //
    
    //! @brief    Checks if addr serves as the first command address.
    bool firstCommandAddr(u32 addr) { return (addr & 0x7FF) == 0x555; }

    //! @brief    Checks if addr serves as the second command address.
    bool secondCommandAddr(u32 addr) { return (addr & 0x7FF) == 0x2AA; }

    //! @brief    Performs a "Byte Program" operation
    bool doByteProgram(u32 addr, u8 value);
    
    //! @brief    Convenience wrapper with bank,offset addressing
    bool doByteProgram(unsigned bank, u16 addr, u8 value) {
        assert(isBankNumber(bank)); return doByteProgram(bank * 0x2000 + addr, value); }
    
    //! @brief    Performs a "Sector Erase" operation
    void doSectorErase(u32 addr);
    
    //! @brief    Convenience wrapper with bank,offset addressing
    void doSectorErase(unsigned bank, u16 addr) {
        assert(isBankNumber(bank)); doSectorErase(bank * 0x2000 + addr); }
    
    //! @brief    Performs a "Chip Erase" operation
    void doChipErase();
};

#endif 
