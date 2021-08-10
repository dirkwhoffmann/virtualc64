// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// This implementation is based mainly by the following documents:
// Schematics and explanation by Martin Sikström:
// https://people.kth.se/~e93_msi/c64/expert.html

#include "config.h"
#include "C64.h"

Expert::Expert(C64 &ref) : Cartridge(ref)
{    
    setSwitch(0);
    
    // Allocate 8KB bytes persistant RAM
    setRamCapacity(0x2000);
    setBattery(true);
    
    trace(CRT_DEBUG, "Expert cartridge created\n");
}

void
Expert::_reset(bool hard)
{
    Cartridge::_reset(hard);
}

void
Expert::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category & dump::State) {
    
        u16 nmi = LO_HI(peekRAM(0x1FFA), peekRAM(0x1FFB));
        u16 irq = LO_HI(peekRAM(0x1FFE), peekRAM(0x1FFF));
        u16 rst = LO_HI(peekRAM(0x1FFC), peekRAM(0x1FFD));
        
        os << tab("active") << bol(active) << std::endl;
        os << tab("switch") << dec(getSwitch()) << std::endl;
        os << tab("NMI vector") << hex(nmi) << std::endl;
        os << tab("IRQ vector") << hex(irq) << std::endl;
        os << tab("Reset vector") << hex(rst) << std::endl;
    }
}

void
Expert::loadChip(isize nr, const CRTFile &crt)
{
    u16 chipSize = crt.chipSize(nr);
    u16 chipAddr = crt.chipAddr(nr);
    u8 *chipData = crt.chipData(nr);
    
    // Check file integrity
    if (nr != 0 || chipSize != 0x2000 || chipAddr != 0x8000) {
        warn("Corrupted CRT file. Aborting.");
        return;
    }

    // Initialize RAM with data from CRT file
    trace(CRT_DEBUG, "Copying file contents into Expert RAM\n");
    assert(getRamCapacity() == chipSize);
    for (isize i = 0; i < chipSize; i++) pokeRAM(i, chipData[i]);
}

u8
Expert::peek(u16 addr)
{
    if (cartridgeRamIsVisible(addr)) {
        
        // Get value from cartridge RAM
        return peekRAM(addr & 0x1FFF);
    
    } else {
        
        // Get value as if no cartridge was attached
        return mem.peek(addr, 1, 1);
    }
}

u8
Expert::peekIO1(u16 addr)
{
    // Any IO1 access disables the cartridge
    active = false;
    
    return 0;
}

u8
Expert::spypeekIO1(u16 addr) const
{
    return 0;
}

void
Expert::poke(u16 addr, u8 value)
{
    if (cartridgeRamIsVisible(addr)) {
        
        // Write value into cartridge RAM if it is write enabled
        if (cartridgeRamIsWritable(addr)) {
            pokeRAM(addr & 0x1FFF, value);
        }
    
    } else {
    
        // Write value as if no cartridge was attached
        mem.poke(addr, value, 1, 1);
    }
}

void
Expert::pokeIO1(u16 addr, u8 value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    
    trace(CRT_DEBUG, "Expert::pokeIO1\n");
    
    // Any IO1 access disabled the cartridge
    active = false;
}

const string
Expert::getButtonTitle(isize nr) const
{
    return nr == 1 ? "Reset" : nr == 2 ? "ESM" : "";
}

void
Expert::pressButton(isize nr)
{
    assert(nr <= numButtons());
    trace(CRT_DEBUG, "Pressing %s button.\n", getButtonTitle(nr).c_str());
    
    suspended {
        
        switch (nr) {
                
            case 1: // Reset
                
                if (switchInOnPosition()) { active = true; }
                resetWithoutDeletingRam();
                break;
                
            case 2: // ESM (Freeze)
                
                if (switchInOnPosition()) { active = true; }
                
                /* The Expert cartridge uses two three-state buffers in parallel
                 * to force the NMI line high, even if a program leaves it low
                 * to protect itself against freezers. The following code is
                 * surely not accurate, but it forces an NMI a trigger,
                 * regardless of the current value of the NMI line.
                 */
                u8 oldLine = cpu.nmiLine;
                u8 newLine = oldLine | INTSRC_EXP;
                
                cpu.releaseNmiLine((IntSource)0xFF);
                cpu.pullDownNmiLine((IntSource)newLine);
                cpu.releaseNmiLine(INTSRC_EXP);
                break;
        }
    }
}



const string
Expert::getSwitchDescription(i8 pos) const
{
    return (pos == -1) ? "Prg" : (pos == 0) ? "Off" : (pos == 1) ? "On" : "";
}

bool
Expert::cartridgeRamIsVisible(u16 addr) const
{
    if (addr < 0x8000) {
        assert(false); // Should never be called for this address space
        return false;
    }
    if (addr < 0xA000) {
        return switchInPrgPosition() || (switchInOnPosition() && active);
    }
    if (addr < 0xE000) {
        return false;
    }
    return switchInOnPosition() && active;
}

bool
Expert::cartridgeRamIsWritable(u16 addr) const
{
    return isROMLaddr(addr);
}

void
Expert::updatePeekPokeLookupTables()
{
    /* Setting up faked Ultimax mode. We let the Game and Exrom line as they
     * are, but reroute all access to ROML and ROMH into the cartridge.
     */
    
    // Reroute ROML
     mem.peekSrc[0x8] = mem.pokeTarget[0x8] = M_CRTLO;
     mem.peekSrc[0x9] = mem.pokeTarget[0x9] = M_CRTLO;

    // Reroute ROMH
     mem.peekSrc[0xE] = mem.pokeTarget[0xE] = M_CRTLO;
     mem.peekSrc[0xF] = mem.pokeTarget[0xF] = M_CRTLO;
}

void
Expert::nmiWillTrigger()
{
    // Activate cartridge if switch is in 'ON' position
    if (switchInOnPosition()) { active = 1; }
}
