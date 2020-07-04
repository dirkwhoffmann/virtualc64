// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// This implementation is based mainly by the following documents:
// Schematics and explanation by Martin Sikström:
// https://people.kth.se/~e93_msi/c64/expert.html

#include "C64.h"

Expert::Expert(C64 *c64, C64 &ref) : Cartridge(c64, ref)
{
    setDescription("Expert");
    
    active = false;
    setSwitch(0);
    
    // Allocate 8KB bytes persistant RAM
    setRamCapacity(0x2000);
    setPersistentRam(true);
    
    debug(CRT_DEBUG, "Expert cartridge created\n");
}

void
Expert::reset()
{
    Cartridge::reset();
}

void
Expert::dump()
{
    Cartridge::dump();
    
    msg("               active: %d\n", active);
    msg("             switch: %d ", getSwitch());
    if (switchInPrgPosition()) msg("(PRG)\n");
    if (switchInOffPosition()) msg("(OFF)\n");
    if (switchInOnPosition()) msg("(ON)\n");
    msg("         NMI vector: %04X\n", LO_HI(peekRAM(0x1FFA), peekRAM(0x1FFB)));
    msg("         IRQ vector: %04X\n", LO_HI(peekRAM(0x1FFE), peekRAM(0x1FFF)));
    msg("       Reset vector: %04X\n", LO_HI(peekRAM(0x1FFC), peekRAM(0x1FFD)));
}

size_t
Expert::stateSize()
{
    return Cartridge::stateSize() + 1;
}

void
Expert::didLoadFromBuffer(u8 **buffer)
{
    Cartridge::didLoadFromBuffer(buffer);
    active = read8(buffer);
}

void
Expert::didSaveToBuffer(u8 **buffer)
{
    Cartridge::didSaveToBuffer(buffer);
    write8(buffer, (u8)active);
}

void
Expert::loadChip(unsigned nr, CRTFile *c)
{
    u16 chipSize = c->chipSize(nr);
    u16 chipAddr = c->chipAddr(nr);
    u8 *chipData = c->chipData(nr);
    
    if (nr != 0 || chipSize != 0x2000 || chipAddr != 0x8000) {
        warn("Corrupted CRT file. Aborting.");
        return;
    }

    assert(getRamCapacity() == chipSize);

    // Initialize RAM with data from CRT file
    debug(CRT_DEBUG, "Copying file contents into Expert RAM\n");
    for (unsigned i = 0; i < chipSize; i++) {
        pokeRAM(i, chipData[i]);
    }
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
    
    debug(CRT_DEBUG, "Expert::pokeIO1\n");
    
    // Any IO1 access disabled the cartridge
    active = false;
}

const char *
Expert::getButtonTitle(unsigned nr)
{
    return (nr == 1) ? "Reset" : (nr == 2) ? "ESM" : NULL;
}

void
Expert::pressButton(unsigned nr)
{
    assert(nr <= numButtons());
    debug(CRT_DEBUG, "Pressing %s button.\n", getButtonTitle(nr));
    
    vc64.suspend();
    
    switch (nr) {
            
        case 1: // Reset
            
            if (switchInOnPosition()) { active = true; }
            resetWithoutDeletingRam();
            break;
            
        case 2: // ESM (Freeze)
            
            if (switchInOnPosition()) { active = true; }
            
            // The Expert cartridge uses two three-state buffers in parallel to
            // force the NMI line high, even if a program leaves it low to
            // protect itself against freezers. The following code is surely
            // not accurate, but it forces an NMI a trigger, regardless of the
            // current value of the NMI line.
            
            u8 oldLine = cpu.nmiLine;
            u8 newLine = oldLine | CPU::INTSRC_EXPANSION;
            
            cpu.releaseNmiLine((CPU::IntSource)0xFF);
            cpu.pullDownNmiLine((CPU::IntSource)newLine);
            cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
            break;
    }
    
    vc64.resume();
}



const char *
Expert::getSwitchDescription(i8 pos)
{
    return (pos == -1) ? "Prg" : (pos == 0) ? "Off" : (pos == 1) ? "On" : NULL;
}

bool
Expert::cartridgeRamIsVisible(u16 addr)
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
Expert::cartridgeRamIsWritable(u16 addr)
{
    return isROMLaddr(addr);
}

void
Expert::updatePeekPokeLookupTables()
{
    // Setting up faked Ultimax mode. We let the Game and Exrom line as they
    // are, but reroute all access to ROML and ROMH into the cartridge.
    
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
