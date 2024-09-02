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
#include "C64.h"

namespace vc64 {

Reu::Reu(C64 &ref, isize kb) : Cartridge(ref), kb(kb)
{
    // The RAM capacity must be a power of two between 128 and 16384
    if ((kb & (kb - 1)) || kb < 128 || kb > 16384) {
        throw Error(VC64ERROR_OPT_INV_ARG, "128, 256, 512, ..., 16384");
    }

    setRamCapacity(KB(kb));
}

void
Reu::_didReset(bool hard)
{
    // Initialize the status register
    sr = isREU1700() ? 0x00 : 0x10;

    // Initialize the command register
    cr = 0x10;

    // Initialize the length register
    tlen = 0xFFFF;
}

void
Reu::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    Cartridge::_dump(category, os);
    os << std::endl;

    if (category == Category::State) {

        string mode[4] = { "STASH", "FETCH", "SWAP", "VERIFY" };
        auto model = isREU1700() ? "1700" : isREU1764() ? "1764" : "1750";

        os << tab("Model");
        os << "REU " << model << std::endl;
        os << tab("Capacity");
        os << dec(getRamCapacity() / 1024) << " KB" << std::endl;
        os << tab("Status Register");
        os << hex(sr) << std::endl;
        os << tab("Command Register");
        os << hex(cr) << std::endl;
        os << tab("C64 Base Address");
        os << hex(c64Base) << std::endl;
        os << tab("REU Base Address");
        os << hex(reuBase) << std::endl;
        os << tab("Upper bank bits" );
        os << hex(upperBankBits) << std::endl;
        os << tab("Transfer Length");
        os << hex(tlen) << std::endl;
        os << tab("Interrupt Mask Register");
        os << hex(imr) << std::endl;
        os << tab("Address Control Register");
        os << hex(acr) << std::endl;

        os << std::endl;
        os << tab("Wrap mask");
        os << hex(wrapMask()) << std::endl;
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

            // Release interrupt line
            cpu.releaseIrqLine(INTSRC_EXP);

            break;

        default:
            break;

    }

    debug(REU_DEBUG, "peekIO2(%x) = %02X\n", addr, result);
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

            result = (u8)HI_WORD(reuBase) | 0xF8;
            break;

        case 0x07:  // Transfer Length (LSB)

            result = LO_BYTE(tlen);
            break;

        case 0x08:  // Transfer Length (MSB)

            result = HI_BYTE(tlen);
            break;

        case 0x09:  // Interrupt Mask

            result = imr | 0x1F;
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
    debug(REU_DEBUG, "pokeIO2(%x,%x)\n", addr, value);

    switch (addr & 0x1F) {

        case 0x00:  // Status Register (read only)

            break;

        case 0x01:  // Command Register

            cr = value;

            if (GET_BIT(cr,7) && ff00Enabled()) {

                debug(REU_DEBUG, "Preparing for DMA...\n");
            }
            if (GET_BIT(cr,7) && ff00Disabled()) {

                debug(REU_DEBUG, "Initiating DMA...\n");
                prepareDma();
            }
            break;

        case 0x02:  // C64 Base Address (LSB)

            c64Base = (u16)REPLACE_LO(c64Base, value);
            break;

        case 0x03:  // C64 Base Address (MSB)

            c64Base = (u16)REPLACE_HI(c64Base, value);
            break;

        case 0x04:  // REU Base Address (LSB)

            reuBase = (u32)REPLACE_LO(reuBase, value);
            break;

        case 0x05:  // REU Base Address (MSB)

            reuBase = (u32)REPLACE_HI(reuBase, value);
            break;

        case 0x06:  // REU Bank

            reuBase = (u32)REPLACE_HI_WORD(reuBase, value & 0b111);

            switch (getRamCapacity()) {

                case KB(128):   upperBankBits = 0; break;
                case KB(256):   upperBankBits = 0; break;
                case KB(512):   upperBankBits = 0; break;
                case KB(1024):  upperBankBits = (value & 0b00001000) << 16; break;
                case KB(2048):  upperBankBits = (value & 0b00011000) << 16; break;
                case KB(4096):  upperBankBits = (value & 0b00111000) << 16; break;
                case KB(8192):  upperBankBits = (value & 0b01111000) << 16; break;
                case KB(16384): upperBankBits = (value & 0b11111000) << 16; break;

                default:
                    dump(Category::State);
                    fatalError;
            }
            break;

        case 0x07:  // Transfer Length (LSB)

            tlen = (u16)REPLACE_LO(tlen, value);
            break;

        case 0x08:  // Transfer Length (MSB)

            tlen = (u16)REPLACE_HI(tlen, value);
            break;

        case 0x09:  // Interrupt Mask

            imr = value;
            triggerEndOfBlockIrq();
            triggerVerifyErrorIrq();
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
    debug(REU_DEBUG, "poke(%x,%x)\n", addr, value);
    assert((addr & 0xF000) == 0xF000);

    if (addr == 0xFF00 && isArmed()) {

        // Initiate DMA
        prepareDma();

    } else {

        // Route the write access back
        mem.poke(addr, value, memTypeF);
    }
}

u8
Reu::readFromC64Ram(u16 addr)
{
    return addr <= 2 ? mem.peek(addr, M_RAM) : mem.peek(addr);
}

void
Reu::writeToC64Ram(u16 addr, u8 value)
{
    addr <= 2 ? mem.poke(addr, value, M_RAM) : mem.poke(addr, value);
}

u8
Reu::readFromReuRam(u32 addr)
{
    addr |= upperBankBits;

    if (addr < u32(getRamCapacity())) {

        bus = peekRAM(addr);
    }

    return bus;
}

void
Reu::writeToReuRam(u32 addr, u8 value)
{
    addr |= upperBankBits;

    if (addr < u32(getRamCapacity())) {

        bus = value;
        pokeRAM(addr, value);
    }
}

void
Reu::prepareDma()
{
    if (REU_DEBUG) { dump(Category::Dma, std::cout); }

    c64Addr = c64Base;
    reuAddr = reuBase;
    isize len = tlen ? tlen : 0x10000;

    // Freeze the CPU
    // cpu.pullDownRdyLine(INTSRC_EXP);

    // Schedule the first event
    c64.scheduleRel<SLOT_EXP>(1, EXP_REU_PREPARE, len);
}

bool
Reu::doDma(EventID id)
{
    u8 c64Val, reuVal;

    switch (id) {

        case EXP_REU_STASH:

            c64Val = readFromC64Ram(c64Addr);
            writeToReuRam(reuAddr, c64Val);

            // debug(REU_DEBUG,"(%x, %02x) -> %x\n", memAddr, c64Val, reuAddr);

            if (memStep()) incMemAddr(c64Addr);
            if (reuStep()) incReuAddr(reuAddr);
            break;

        case EXP_REU_FETCH:

            reuVal = readFromReuRam(reuAddr);
            writeToC64Ram(c64Addr, reuVal);

            // debug(REU_DEBUG,"%x <- (%x, %02x)\n", memAddr, reuAddr, reuVal);

            if (memStep()) incMemAddr(c64Addr);
            if (reuStep()) incReuAddr(reuAddr);
            break;

        case EXP_REU_SWAP:

            c64Val = readFromC64Ram(c64Addr);
            reuVal = readFromReuRam(reuAddr);

            writeToC64Ram(c64Addr, reuVal);
            writeToReuRam(reuAddr, c64Val);

            if (memStep()) incMemAddr(c64Addr);
            if (reuStep()) incReuAddr(reuAddr);
            break;

        case EXP_REU_VERIFY:

            c64Val = readFromC64Ram(c64Addr);
            reuVal = readFromReuRam(reuAddr);

            if (c64Val != reuVal) {

                debug(REU_DEBUG, "Verify error: (%x,%02x) <-> (%x,%02x)\n",
                      c64Addr, c64Val, reuAddr, reuVal);

                // Set the "Fault" bit
                SET_BIT(sr, 5);

                // Trigger interrupt if enabled
                triggerVerifyErrorIrq();

                return false;
            }

            if (memStep()) incMemAddr(c64Addr);
            if (reuStep()) incReuAddr(reuAddr);
            break;

        default:
            fatalError;
    }

    return true;
}

void 
Reu::finalizeDma(EventID id)
{
    // Set the "End of Block" bit
    SET_BIT(sr, 6);

    if (!autoloadEnabled()) {

        c64Base = c64Addr;
        reuBase = reuAddr;
    }

    triggerEndOfBlockIrq();

    // Release the CPU
    cpu.releaseRdyLine(INTSRC_EXP);
}

void 
Reu::processEvent(EventID id)
{
    if (id == EXP_REU_PREPARE) {

        cpu.pullDownRdyLine(INTSRC_EXP);

        c64.scheduleRel<SLOT_EXP>(1, EXP_REU_PREPARE2, c64.data[SLOT_EXP]);
        return;
    }

    if (id == EXP_REU_PREPARE2) {

        // Freeze the CPU
        cpu.pullDownRdyLine(INTSRC_EXP);

        switch (cr & 0x3) {

            case 0: id = EXP_REU_STASH; break;
            case 1: id = EXP_REU_FETCH; break;
            case 2: id = EXP_REU_SWAP; break;
            case 3: id = EXP_REU_VERIFY; break;
        }
    }

    auto remaining = c64.data[SLOT_EXP];

    // Determine the number of bytes to transfer
    auto todo = std::min(remaining, i64(bytesPerDmaCycle()));

    // Perform DMA if VICII does not block the bus
    if (!vic.baLine.delayed()) for (; todo; todo--, remaining--) doDma(id);

    if (remaining) {

        // Prepare the next event
        c64.scheduleInc<SLOT_EXP>(1, id, remaining);

    } else {

        // Finalize DMA if this was the last cycle
        finalizeDma(id);
        c64.cancel<SLOT_EXP>();
    }
}

void
Reu::triggerEndOfBlockIrq()
{
    if (irqEnabled() && irqOnEndOfBlock() && GET_BIT(sr, 6)) {

        sr |= 0x80;
        cpu.pullDownIrqLine(INTSRC_EXP);
    }
}

void
Reu::triggerVerifyErrorIrq()
{
    if (irqEnabled() && irqOnVerifyError() && GET_BIT(sr, 5)) {

        sr |= 0x80;
        cpu.pullDownIrqLine(INTSRC_EXP);
    }
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
     *  before the DMA takes place. The FF00 option is cleared each time it is
     *  used."
     */

    // Remember the original mapping
    memTypeF = mem.pokeTarget[0xF];

    // Reroute accesses to this cartridge
    mem.pokeTarget[0xF] = M_CRTHI;
}

}
