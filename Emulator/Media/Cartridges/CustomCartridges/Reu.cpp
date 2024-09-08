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

    traits.memory = KB(kb);
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
    tlength = tlengthLatched = 0xFFFF;

    // Experimental
    bus = 0x00; // 0xFF;
}

void
Reu::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    string mode[4] = { "STASH", "FETCH", "SWAP", "VERIFY" };

    Cartridge::_dump(category, os);

    if (category == Category::State) {

        auto model = isREU1700() ? "1700" : isREU1764() ? "1764" : "1750";

        os << std::endl;
        os << tab("Model");
        os << "REU " << model << std::endl;
        os << tab("Capacity");
        os << dec(getRamCapacity() / 1024) << " KB" << std::endl;
        os << tab("Status Register");
        os << hex(sr) << std::endl;
        os << tab("Command Register");
        os << hex(cr) << std::endl;
        os << tab("C64 Base Address");
        os << hex(c64Base) << " (latched: " << hex(c64BaseLatched) << ")" << std::endl;
        os << tab("REU Base Address");
        os << hex(reuBase) << " (latched: " << hex(reuBaseLatched) << ")" << std::endl;
        os << tab("Bank register");
        os << hex(reuBank) << " (latched: " << hex(reuBankLatched) << ")" << std::endl;
        os << tab("Transfer Length Register");
        os << hex(tlength) << " (latched: " <<  hex(tlengthLatched) << ")" << std::endl;
        os << tab("Interrupt Mask Register");
        os << hex(imr) << std::endl;
        os << tab("Address Control Register");
        os << hex(acr) << std::endl;
        os << std::endl;
        os << tab("Upper bank bits" );
        os << hex(upperBankBits) << std::endl;
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

    if (category == Category::Dma) {

        string symb[4] = { "->", "<-", "<->", "==" };

        os << mode[cr & 3] << ": ";
        os << "C64: " << hex(c64Base) << (memStep() ? "+ " : " ");
        os << symb[cr & 3] << " ";
        os << "REU: " << dec(reuBank) << ":" << hex(reuBase) << (reuStep() ? "+ " : " ");
        os << "Len: " << dec(tlength);
        os << std::endl;
    }
}

bool 
Reu::isActive() const
{
    return c64.hasEvent<SLOT_EXP>();
}

void
Reu::eraseRAM()
{
    auto capacity = (u32)getRamCapacity();

    for (u32 i = 0; i < capacity; i++) {

        u8 invert = (i & 0x20000) ? 0xFF : 0x00;
        pokeRAM(i, (((i + 1) & 0b10) ? 0x00 : 0xFF) ^ invert);
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

    debug(REU_DEBUG >= 2, "peekIO2(%x) = %02X\n", addr, result);
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

        case 0x06:  // Bank

            result = reuBank | 0xF8;
            break;

        case 0x07:  // Transfer Length (LSB)

            result = LO_BYTE(tlength);
            break;

        case 0x08:  // Transfer Length (MSB)

            result = HI_BYTE(tlength);
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
    debug(REU_DEBUG >= 2, "pokeIO2(%x,%x)\n", addr, value);

    switch (addr & 0x1F) {

        case 0x00:  // Status Register (read only)

            break;

        case 0x01:  // Command Register

            cr = value;

            if (GET_BIT(cr,7) && ff00Enabled()) {

                // debug(REU_DEBUG, "Preparing for DMA [Mode %d]...\n", cr & 0x3);
            }
            if (GET_BIT(cr,7) && ff00Disabled()) {

                // debug(REU_DEBUG, "Initiating DMA [Mode %d]...\n", cr & 0x3);
                initiateDma();
            }
            break;

        case 0x02:  // C64 Base Address (LSB)

            c64BaseLatched = (u16)REPLACE_LO(c64BaseLatched, value);
            c64Base = c64BaseLatched;
            break;

        case 0x03:  // C64 Base Address (MSB)

            c64BaseLatched = (u16)REPLACE_HI(c64BaseLatched, value);
            c64Base = c64BaseLatched;
            break;

        case 0x04:  // REU Base Address (LSB)

            reuBaseLatched = (u16)REPLACE_LO(reuBaseLatched, value);
            reuBase = reuBaseLatched;
            break;

        case 0x05:  // REU Base Address (MSB)

            reuBaseLatched = (u16)REPLACE_HI(reuBaseLatched, value);
            reuBase = reuBaseLatched;
            break;

        case 0x06:  // REU Bank

            reuBankLatched = value & 0b111;
            reuBank = reuBankLatched;

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

            tlengthLatched = (u16)REPLACE_LO(tlengthLatched, value);
            tlength = tlengthLatched;
            break;

        case 0x08:  // Transfer Length (MSB)

            tlengthLatched = (u16)REPLACE_HI(tlengthLatched, value);
            tlength = tlengthLatched;
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
    assert((addr & 0xF000) == 0xF000);

    if (addr != 0xFF00) {

        mem.poke(addr, value, memTypeF);

    } else {

        trace(REU_DEBUG, "poke($FF00,%02X)\n", value);

        if (isActive()) {

            debug(REU_DEBUG, "Ignoring write to $FF00. REU already active\n");
            return;
        }

        if (!isArmed()) {

            debug(REU_DEBUG, "Ignoring write to $FF00. REU not armed\n");
            mem.poke(addr, value, memTypeF);
            return;
        }

        debug(REU_DEBUG, "Starting REU via FF00 trigger\n");
        if (memTypeF != M_RAM) mem.poke(addr, value, memTypeF);

        initiateDma();
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

    } else if (!floating(addr)) {

        bus = peekRAM(mapAddr(addr));
    }

    return bus;
}

void
Reu::writeToReuRam(u32 addr, u8 value)
{
    addr |= upperBankBits;

    bus = value;

    if (addr < u32(getRamCapacity())) {

        pokeRAM(addr, value);

    } else if (!floating(addr)) {

        pokeRAM(mapAddr(addr), value);
    }
}

bool 
Reu::floating(u32 addr) const
{
    auto bank = [&](u32 addr) { return addr >> 16; };

    switch (getRamCapacity()) {

        case KB(256):   return (bank(addr) % 8) >= 4;
        default:        return false;
    }
}

u32
Reu::mapAddr(u32 addr) const
{
    auto capacity = getRamCapacity();

    switch (capacity) {

        case KB(256):   return addr & 0x07FFFF;
        default:        return addr & (capacity - 1);
    }
}

void 
Reu::incMemAddr()
{
    c64Base = U16_ADD(c64Base, 1);
}

void
Reu::incReuAddr()
{
    u32 expanded = U32_ADD((u32)reuBank << 16 | reuBase, 1) & wrapMask();

    reuBank = (u8)HI_WORD(expanded);
    reuBase = LO_WORD(expanded);
}

void 
Reu::execute()
{
    // Quick exit
    if (action == EVENT_NONE) return;

    // Sniff the BA line
    sniffBA();

    for (isize i = 0; i < expansionPort.getConfig().reuSpeed; i++) {

        // Emulate wait state if necessary
        if (waitStates) { waitStates--; return; }

        // Only proceed if an action is scheduled
        if (action == EVENT_NONE) return;

        // Execute the pending action
        execute(action);
    }
}

void 
Reu::execute(EventID id)
{
    switch (id) {

        case EXP_REU_INITIATE:

            if (REU_DEBUG) { trace(true, ""); dump(Category::Dma, std::cout); }

            // Update control register bits
            cr = (cr & ~CR::EXECUTE) | CR::FF00_DISABLE;

            // Freeze the CPU
            cpu.pullDownRdyLine(INTSRC_EXP);

            // Schedule the first DMA event
            schedule(EXP_REU_PREPARE);
            break;

        case EXP_REU_PREPARE:

            // cpu.pullDownRdyLine(INTSRC_EXP);

            switch (cr & 0x3) {

                case 0: id = EXP_REU_STASH; break;
                case 1: id = EXP_REU_FETCH; break;
                case 2: id = EXP_REU_SWAP; break;
                case 3: id = EXP_REU_VERIFY; break;
            }
            [[fallthrough]];

        case EXP_REU_STASH:
        case EXP_REU_FETCH:
        case EXP_REU_SWAP:
        case EXP_REU_VERIFY:
        {
            trace(REU_DEBUG > 3, "%d%d : ", ba[1], ba[0]);

            // Only proceed if the bus is available
            if (busIsBlocked(id)) {

                if (REU_DEBUG > 3) printf("BLOCKED\n");
                break;
            }

            // Perform a DMA cycle
            auto remaining = doDma(id);

            if (id == EXP_REU_STASH && REU_DEBUG > 3) printf("Stashing\n");
            if (id == EXP_REU_FETCH && REU_DEBUG > 3) printf("Fetching %02x\n", reuVal);

            // Set or clear the END_OF_BLOCK_BIT
            tlength == 1 ? SET_BIT(sr, 6) : CLR_BIT(sr, 6);

            // Process the event again in the next cycle if DMA continues
            if (remaining > 0) break;

            // Delay for one cycle (if needed)
            if (waitStates) { schedule(EXP_REU_AUTOLOAD); break; }
            [[fallthrough]];
        }
        case EXP_REU_AUTOLOAD:

            cpu.releaseRdyLine(INTSRC_EXP);

            if (autoloadEnabled()) {

                debug(REU_DEBUG, "Autoloading...\n");

                // Reload values from shadow registers
                c64Base = c64BaseLatched;
                reuBase = reuBaseLatched;
                reuBank = reuBankLatched;
                tlength = tlengthLatched;

                // Emulate a proper delay
                if (id != EXP_REU_SWAP) {

                    schedule(EXP_REU_FINALIZE, 3);
                    break;
                }
            }
            [[fallthrough]];

        case EXP_REU_FINALIZE:

            finalizeDma(id);
            c64.cancel<SLOT_EXP>();
            schedule(EVENT_NONE);
            break;

        default:
            fatalError;
    }
}

void
Reu::sniffBA()
{
    // Scan the BA line
    ba[0] = vic.baLine.current();
    ba[1] = vic.baLine.readWithDelay(1);

    // Check if DMA for sprite 0 was off and has been switched on
    if (c64.rasterCycle == 54) lateBA = !GET_BIT(vic.spriteDmaOnOff, 1);

    /* From Denise's vicii.h:
     *
     * "of course expansion port sees the same BA state like CPU RDY line.
     *  but there is a known case, when BA calculation takes more time within cycle.
     *  for CPU it doesn't matter, because it checks later in cycle.
     *  REU seems to check this sooner and can't recognize BA in this special cycle."
     */
    if (c64.rasterCycle == 55 && lateBA) { ba[0] = false; }
}

bool 
Reu::busIsBlocked(EventID id) const
{
    switch (id) {

        case EXP_REU_FETCH:

            if (tlength == 1) return ba[0];
            return ba[0] && ba[1];

        case EXP_REU_STASH:
        case EXP_REU_SWAP:
        case EXP_REU_VERIFY:

            return ba[0];

        default:
            fatalError;
    }
}

void
Reu::initiateDma()
{
    schedule(EXP_REU_INITIATE);
}

isize
Reu::doDma(EventID id)
{
    switch (id) {

        case EXP_REU_STASH:

            c64Val = readFromC64Ram(c64Base);
            writeToReuRam((u32)reuBank << 16 | reuBase, c64Val);

            if (memStep()) incMemAddr();
            if (reuStep()) incReuAddr();
            break;

        case EXP_REU_FETCH:

            reuVal = readFromReuRam((u32)reuBank << 16 | reuBase);
            writeToC64Ram(c64Base, reuVal);

            if (memStep()) incMemAddr();
            if (reuStep()) incReuAddr();

            prefetch((u32)reuBank << 16 | reuBase);
            break;

        case EXP_REU_SWAP:

            // Only proceed every second cycle
            if ((swapff = !swapff) == true) {

                c64Val = readFromC64Ram(c64Base);
                reuVal = readFromReuRam((u32)reuBank << 16 | reuBase);

                return tlength;
            }

            writeToC64Ram(c64Base, reuVal);
            writeToReuRam((u32)reuBank << 16 | reuBase, c64Val);

            if (memStep()) incMemAddr();
            if (reuStep()) incReuAddr();

            prefetch((u32)reuBank << 16 | reuBase);
            break;

        case EXP_REU_VERIFY:

            c64Val = readFromC64Ram(c64Base);
            reuVal = readFromReuRam((u32)reuBank << 16 | reuBase);

            if (memStep()) incMemAddr();
            if (reuStep()) incReuAddr();

            if (c64Val != reuVal) {

                debug(REU_DEBUG, "Verify error: (%x,%02x) <-> (%x,%02x)\n",
                      c64Base, c64Val, (u32)reuBank << 16 | reuBase, reuVal);

                // Set the Fault bit
                SET_BIT(sr, 5);

                // Trigger interrupt if enabled
                triggerVerifyErrorIrq();

                waitStates = tlength == 1 ? 0 : 1;
                if (tlength != 1) U16_DEC(tlength, 1);
                return -1;
            }
            break;

        default:
            fatalError;
    }

    if (tlength == 1) {

        triggerEndOfBlockIrq();
        return 0;
    }

    U16_DEC(tlength, 1);
    return tlength;
}

void 
Reu::finalizeDma(EventID id)
{
    // if (!verifyError) triggerEndOfBlockIrq();

    // Release the CPU
    cpu.releaseRdyLine(INTSRC_EXP);
}

void
Reu::triggerEndOfBlockIrq()
{
    if (irqEnabled() && irqOnEndOfBlock() && GET_BIT(sr, 6)) {

        sr |= 0x80;
        cpu.pullDownIrqLine(INTSRC_EXP);

        debug(REU_DEBUG, "End-of-block IRQ triggered (sr = %02x)\n", sr);
    }
}

void
Reu::triggerVerifyErrorIrq()
{
    if (irqEnabled() && irqOnVerifyError() && GET_BIT(sr, 5)) {

        sr |= 0x80;
        cpu.pullDownIrqLine(INTSRC_EXP);

        debug(REU_DEBUG, "Verify-error IRQ triggered (sr = %02x)\n", sr);
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
