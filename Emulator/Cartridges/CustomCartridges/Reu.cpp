// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"

void
Reu::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)

    if (!getBattery()) {
        trace(CRT_DEBUG, "Erasing REU contents\n");
        eraseRAM(0);
    } else {
        trace(CRT_DEBUG, "Preserving REU contents\n");
    }
}

void
Reu::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    Cartridge::_dump(category, os);

    if (category == Category::State) {

        os << std::endl;
        os << tab("Status Register");
        os << hex(sr) << std::endl;
        os << tab("Command Register");
        os << hex(cr) << std::endl;
        os << tab("C64 Base Address");
        os << hex(c64Base) << std::endl;
        os << tab("REU Base Address");
        os << hex(reuBase) << std::endl;
        os << tab("REU Bank");
        os << hex(bank) << std::endl;
        os << tab("Transfer Length");
        os << hex(tlen) << std::endl;
        os << tab("Interrupt Mask Register");
        os << hex(mask) << std::endl;
        os << tab("Address Control Register");
        os << hex(acr) << std::endl;
    }
}

u8
Reu::peekIO2(u16 addr)
{
    u8 result;

    switch (addr & 0x1F) {

        case 0x00:  // Status Register (read only)

            result = sr;
            break;

        case 0x01:  // Command Register

            result = cr;
            break;

        case 0x02:  // C64 Base Address (LSB)

            result = LO_BYTE(c64Base);
            break;

        case 0x03:  // C64 Base Address (MSB)

            result = HI_BYTE(c64Base);
            break;

        case 0x04:  // REU Base Address (LSB)

            result = LO_BYTE(reuBase);
            break;

        case 0x05:  // REU Base Address (MSB)

            result = HI_BYTE(reuBase);
            break;

        case 0x06:  // REU Bank

            result = bank | 0xF8;
            break;

        case 0x07:  // Transfer Length (LSB)

            result = LO_BYTE(tlen);
            break;

        case 0x08:  // Transfer Length (MSB)

            result = HI_BYTE(tlen);
            break;

        case 0x09:  // Interrupt Mask

            result = mask | 0x1F;
            break;

        case 0x0A:  // Address Control Register

            result = acr | 0x3F;
            break;

        default:

            result = 0xFF;
    }

    debug(REU_DEBUG,"peekIO2(%x) = %02X\n", addr, result);
    return result;
}

u8
Reu::spypeekIO2(u16 addr) const
{
    u8 result;

    switch (addr & 0x1F) {

        case 0x00:  // Status Register (read only)

            result = sr;
            break;

        case 0x01:  // Command Register

            result = cr;
            break;

        case 0x02:  // C64 Base Address (LSB)

            result = LO_BYTE(c64Base);
            break;

        case 0x03:  // C64 Base Address (MSB)

            result = HI_BYTE(c64Base);
            break;

        case 0x04:  // REU Base Address (LSB)

            result = LO_BYTE(reuBase);
            break;

        case 0x05:  // REU Base Address (MSB)

            result = HI_BYTE(reuBase);
            break;

        case 0x06:  // REU Bank

            result = bank | 0xF8;
            break;

        case 0x07:  // Transfer Length (LSB)

            result = LO_BYTE(tlen);
            break;

        case 0x08:  // Transfer Length (MSB)

            result = HI_BYTE(tlen);
            break;

        case 0x09:  // Interrupt Mask

            result = mask | 0x1F;
            break;

        case 0x0A:  // Address Control Register

            result = acr | 0x3F;
            break;

        default:

            result = 0xFF;
    }

    return result;
}

void
Reu::pokeIO2(u16 addr, u8 value)
{
    debug(REU_DEBUG,"pokeIO2(%x,%x)\n", addr, value);

    switch (addr & 0x1F) {

        case 0x00:  // Status Register (read only)

            break;

        case 0x01:  // Command Register

            cr = value;
            break;

        case 0x02:  // C64 Base Address (LSB)

            c64Base = (u16)REPLACE_LO(c64Base, value);
            break;

        case 0x03:  // C64 Base Address (MSB)

            c64Base = (u16)REPLACE_HI(c64Base, value);
            break;

        case 0x04:  // REU Base Address (LSB)

            reuBase = (u16)REPLACE_LO(reuBase, value);
            break;

        case 0x05:  // REU Base Address (MSB)

            reuBase = (u16)REPLACE_HI(reuBase, value);
            break;

        case 0x06:  // REU Bank

            bank = value;
            break;

        case 0x07:  // Transfer Length (LSB)

            tlen = (u16)REPLACE_LO(tlen, value);
            break;

        case 0x08:  // Transfer Length (MSB)

            tlen = (u16)REPLACE_HI(tlen, value);
            break;

        case 0x09:  // Interrupt Mask

            mask = value;
            break;

        case 0x0A:  // Address Control Register

            acr = value;
            break;

        default:

            break;
    }
}
