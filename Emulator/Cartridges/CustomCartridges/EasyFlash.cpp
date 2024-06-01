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

#include "config.h"
#include "EasyFlash.h"
#include "C64.h"

namespace vc64 {

EasyFlash::EasyFlash(C64 &ref) : Cartridge(ref)
{
    subComponents = std::vector <CoreComponent *> {

        &flashRomL,
        &flashRomH
    };
}

void
EasyFlash::resetCartConfig()
{
    expansionPort.setCartridgeMode(CRTMODE_ULTIMAX);
}

void
EasyFlash::_reset(bool hard)
{
    eraseRAM(0);

    // Make sure peekRomL() and peekRomH() conver the whole range
    mappedBytesL = 0x2000;
    mappedBytesH = 0x2000;
}

void
EasyFlash::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    Cartridge::_dump(category, os);

    if (category == Category::State) {

        os << std::endl;

        os << tab("Bank Register");
        os << hex(bankReg) << std::endl;
        os << tab("Mode Register");
        os << hex(modeReg) << std::endl;
        os << tab("Selected bank");
        os << dec(bank);
        os << tab("GameLine Jumper");
        os << bol(jumper);
    }

    flashRomL.dump(category, os);
    flashRomH.dump(category, os);
}

void
EasyFlash::loadChip(isize nr, const CRTFile &crt)
{
    u16 chipSize = crt.chipSize(nr);
    u16 chipAddr = crt.chipAddr(nr);
    u16 chipBank = crt.chipBank(nr);
    u8 *chipData = crt.chipData(nr);

    if(chipSize != 0x2000) {
        warn("Package %ld has chip size %04X. Expected 0x2000.\n", nr, chipSize);
        return;
    }

    if (isROMLaddr(chipAddr)) {

        trace(CRT_DEBUG, "Loading Rom bank %dL ...\n", chipBank);
        flashRomL.loadBank(chipBank, chipData);
        bank++;

    } else if (isROMHaddr(chipAddr)) {

        trace(CRT_DEBUG, "Loading Rom bank %dH ...\n", bank / 2);
        flashRomH.loadBank(chipBank, chipData);
        bank++;

    } else {

        warn("Package %ld has an invalid load address (%04X).", nr, chipAddr);
        return;
    }
}

u8
EasyFlash::peek(u16 addr)
{
    u8 result;

    if (isROMLaddr(addr)) {

        result = flashRomL.peek(bank, addr & 0x1FFF);
        return result;
    }
    if (isROMHaddr(addr)) {

        result = flashRomH.peek(bank, addr & 0x1FFF);
        return result;
    }

    fatalError;
}

u8
EasyFlash::spypeek(u16 addr) const
{
    u8 result;

    if (isROMLaddr(addr)) {

        result = flashRomL.spypeek(bank, addr & 0x1FFF);
        return result;
    }
    if (isROMHaddr(addr)) {

        result = flashRomH.spypeek(bank, addr & 0x1FFF);
        return result;
    }

    fatalError;
}

void
EasyFlash::poke(u16 addr, u8 value)
{
    Cartridge::poke(addr, value);
}

void
EasyFlash::pokeRomL(u16 addr, u8 value)
{
    trace(CRT_DEBUG, "pokeRomL(%x, %x)\n", addr, value);
    flashRomL.poke(bank, addr & 0x1FFF, value);
}

void
EasyFlash::pokeRomH(u16 addr, u8 value)
{
    trace(CRT_DEBUG, "pokeRomH(%x, %x)\n", addr, value);
    flashRomH.poke(bank, addr & 0x1FFF, value);
}

u8
EasyFlash::peekIO1(u16 addr)
{
    u8 result = (addr & 2) ? (modeReg & 0x87) : bankReg;
    trace(CRT_DEBUG, "peekIO1(%x): %x\n", addr & 0xFF, result);
    return result;
}

u8
EasyFlash::spypeekIO1(u16 addr) const
{
    u8 result = (addr & 2) ? (modeReg & 0x87) : bankReg;
    return result;
}

u8
EasyFlash::peekIO2(u16 addr)
{
    u8 result = peekRAM(addr & 0xFF);
    trace(CRT_DEBUG, "peekIO2(%x): %x\n", addr & 0xFF, result);
    return result;
}

u8
EasyFlash::spypeekIO2(u16 addr) const
{
    u8 result = peekRAM(addr & 0xFF);
    return result;
}

void
EasyFlash::pokeIO1(u16 addr, u8 value)
{
    trace(CRT_DEBUG, "pokeIO1(%x,%x)\n", addr & 0xFF, value);
    (addr & 2) ? pokeModeReg(value) : pokeBankReg(value);
}

void
EasyFlash::pokeIO2(u16 addr, u8 value)
{
    trace(CRT_DEBUG, "pokeIO2(%x,%x)\n", addr & 0xFF, value);
    pokeRAM(addr & 0xFF, value);
}

void
EasyFlash::pokeBankReg(u8 value)
{
    bankReg = value;
    bank = value & 0x3F;
    debug(CRT_DEBUG, "Switching to bank %d\n", bank);
}

void
EasyFlash::pokeModeReg(u8 value)
{
    modeReg = value;
    // c64.signalBreakpoint();

    setLED((value & 0x80) != 0);

    u8 MXG = value & 0x07;
    /* MXG
     * 0 (000) : GAME from jumper, EXROM high (i.e. Ultimax or Off)
     * 1 (001) : Reserved, don’t use this
     * 2 (010) : GAME from jumper, EXROM low (i.e. 16K or 8K)
     * 3 (011) : Reserved, don’t use this
     * 4 (100) : Cartridge ROM off (RAM at $DF00 still available)
     * 5 (101) : Ultimax (Low bank at $8000, high bank at $e000)
     * 6 (110) : 8k Cartridge (Low bank at $8000)
     * 7 (111) : 16k cartridge (Low bank at $8000, high bank at $a000)
     */

    bool exrom;
    bool game;

    debug(CRT_DEBUG, "MXG = %x\n", MXG);
    switch (MXG) {

        case 0b000:
        case 0b001:

            game = jumper;
            exrom = 1;
            break;

        case 0b010:
        case 0b011:

            game = jumper;
            exrom = 0;
            break;

        case 0b100:

            game = 1;
            exrom = 1;
            break;

        case 0b101:

            game = 0;
            exrom = 1;
            break;

        case 0b110:

            game = 1;
            exrom = 0;
            break;

        case 0b111:

            game = 0;
            exrom = 0;
            break;

        default:
            fatalError;
    }

    expansionPort.setGameAndExrom(game, exrom);
}

}
