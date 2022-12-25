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

    sr = 0x10;      // Size = 256KB
    cr = 0x10;      // FF00 option is disabled
    tlen = 0xFFFF;
}

void
Reu::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    Cartridge::_dump(category, os);

    if (category == Category::State) {

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

    if (category == Category::Dma) {

        string mode[4] = { "STASH", "FETCH", "SWAP", "VERIFY" };

        os << tab("Mode");
        os << mode[cr & 3] << std::endl;
        os << tab("Autoload");
        os << bol(autoloadEnabled()) << std::endl;
        os << tab("FF00 decode");
        os << bol(ff00Enabled()) << std::endl;
        os << tab("Irq enabled");
        os << bol(irqEnabled()) << std::endl;
        os << tab("Irq on end of block");
        os << bol(irqOnEndOfBlock()) << std::endl;
        os << tab("Irq on verify error");
        os << bol(irqOnVerifyError()) << std::endl;
        os << tab("C64 address increment");
        os << dec(memStep()) << std::endl;
        os << tab("REU address increment");
        os << dec(reuStep()) << std::endl;
    }
}

u8
Reu::peekIO2(u16 addr)
{
    u8 result = spypeekIO2(addr);

    switch (addr & 0x1F) {

        case 0x00:  // Status Register (read only)

            // Clear bits 5 - 7
            sr &= 0x1F;

            // TODO: CLEAR PENDING INTERRUPT
            break;

        default:
            break;

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

            if (cr & 0x80) {

                debug(REU_DEBUG,"Initiating DMA\n");
                doDma();
            }
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

void
Reu::poke(u16 addr, u8 value)
{
    debug(REU_DEBUG,"poke(%x,%x)\n", addr, value);
    assert((addr & 0xF000) == 0xF000);

    if (addr == 0xFF00) {

        debug(REU_DEBUG,"INTERCEPTED WRITE TO $FF00");
    }

    // Route the write access back
    mem.poke(addr, value, memTypeF);
}

void
Reu::doDma()
{
    if constexpr (REU_DEBUG) { dump(Category::Dma, std::cout); }

    u32 len = tlen ? tlen : 0x10000;
    u16 memAddr = c64Base;
    u32 reuAddr = HI_W_LO_W(bank, reuBase);

    switch (cr & 0x3) {

        case 0: stash(memAddr, reuAddr, len); break;
        case 1: fetch(memAddr, reuAddr, len); break;
        case 2: swap(memAddr, reuAddr, len); break;
        case 3: verify(memAddr, reuAddr, len); break;

        default:
            fatalError;
    }
}

void
Reu::stash(u16 memAddr, u32 reuAddr, u32 len)
{
    debug(REU_DEBUG,"stash(%x,%x,%d)\n", memAddr, reuAddr, len);

    for (isize i = 0, ms = memStep(), rs = reuStep(); i < len; i++) {

        u8 memValue = mem.peek(memAddr);
        pokeRAM(reuAddr, memValue);

        // debug(REU_DEBUG,"(%x, %02x) -> %x\n", memAddr, value, reuAddr);

        if (ms) incMemAddr(memAddr);
        if (rs) incReuAddr(reuAddr);
    }

    // Set the "End of Block" bit
    SET_BIT(sr, 6); 

    // Trigger interrupt if enabled
    // TODO
}

void
Reu::fetch(u16 memAddr, u32 reuAddr, u32 len)
{
    debug(REU_DEBUG,"fetch(%x,%x,%d)\n", memAddr, reuAddr, len);

    for (isize i = 0, ms = memStep(), rs = reuStep(); i < len; i++) {

        u8 reuValue = peekRAM(reuAddr);
        mem.poke(memAddr, reuValue);

        // debug(REU_DEBUG,"%x <- (%x, %02x)\n", memAddr, reuAddr, value);

        if (ms) incMemAddr(memAddr);
        if (rs) incReuAddr(reuAddr);
    }

    // Set the "End of Block" bit
    SET_BIT(sr, 6);

    // Trigger interrupt if enabled
    // TODO


}

void
Reu::swap(u16 memAddr, u32 reuAddr, u32 len)
{
    debug(REU_DEBUG,"swap(%x,%x,%d)\n", memAddr, reuAddr, len);

    for (isize i = 0, ms = memStep(), rs = reuStep(); i < len; i++) {

        u8 memVal = mem.peek(memAddr);
        u8 reuVal = peekRAM(reuAddr);

        // debug(REU_DEBUG, "(%x,%02x) <-> (%x,%02x)\n", memAddr, memVal, reuAddr, reuVal);

        mem.poke(memAddr, reuVal);
        pokeRAM(reuAddr, memVal);

        if (ms) incMemAddr(memAddr);
        if (rs) incReuAddr(reuAddr);
    }

    // Set the "End of Block" bit
    SET_BIT(sr, 6);

    // Trigger interrupt if enabled
    // TODO
}

void
Reu::verify(u16 memAddr, u32 reuAddr, u32 len)
{
    debug(REU_DEBUG,"verify(%x,%x,%d)\n", memAddr, reuAddr, len);

    for (isize i = 0, ms = memStep(), rs = reuStep(); i < len; i++) {

        u8 memVal = mem.peek(memAddr);
        u8 reuVal = peekRAM(reuAddr);

        // debug(REU_DEBUG, "(%x,%02x) == (%x,%02x)\n", memAddr, memVal, reuAddr, reuVal);

        if (memVal != reuVal) {

            debug(REU_DEBUG, "Verify error: (%x,%02x) <-> (%x,%02x)\n", memAddr, memVal, reuAddr, reuVal);

            // Set the "Fault" bit
            SET_BIT(sr, 5);

        }

        if (ms) incMemAddr(memAddr);
        if (rs) incReuAddr(reuAddr);
    }

    // Set the "End of Block" bit
    SET_BIT(sr, 6);

    // Trigger interrupt if enabled
    // TODO
}

void
Reu::updatePeekPokeLookupTables()
{
    /* Reroute all write accesses into memory bank $F to this cartridge in
     * order to intercept accessess to memory address $FF00. This register is
     * utilized by the REU to initiate DMA:
     *
     * "If the FF00 option is enabled, the DMA begins immediately after a
     *  write to address FF00 is detected - providing the execute bit was
     *  previously set in the command register.
     *  The prupose of the FF00 option is to allow the REC to access the RAM
     *  beneath I/O space. The REC registers are I/O mapped on the C64
     *  expansion bus, which means that, without the FF00 option, I/O space
     *  would be enabled when DMA was initiated. This option, therefore, allows
     *  the user to bank out the the C64 I/O space, replacing it with RAM,
     *  before the DMA takes place. The FFOO option is cleared each time it is
     *  used."
     */

    // Remember the original mapping
    memTypeF = mem.pokeTarget[0xF];

    // Reroute accesses to this cartridge
    mem.pokeTarget[0xF] = M_CRTHI;
}
