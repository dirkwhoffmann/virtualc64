// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

VC1541Memory::VC1541Memory(VC1541 *drive)
{
    setDescription("1541MEM");
	debug(3, "  Creating VC1541 memory at %p...\n", this);

    this->drive = drive;
    
    memset(rom, 0, sizeof(rom));
    stack = &ram[0x0100];
    
    // Register snapshot items
    SnapshotItem items[] = {

    { ram,  sizeof(ram), KEEP_ON_RESET },
    { rom,  sizeof(rom), KEEP_ON_RESET },
    { NULL, 0,           0 }};

    registerSnapshotItems(items, sizeof(items));
}

VC1541Memory::~VC1541Memory()
{
	debug(3, "  Releasing VC1541 memory at %p...\n", this);
}

void 
VC1541Memory::reset()
{
    HardwareComponent::reset();
    
    // Initialize RAM with powerup pattern (pattern from Hoxs64)
    for (unsigned i = 0; i < sizeof(ram); i++) {
        ram[i] = (i & 64) ? 0xFF : 0x00;
    }
}

void 
VC1541Memory::dump()
{
	msg("VC1541 Memory:\n");
	msg("--------------\n\n");
	msg("VC1541 ROM :%s loaded\n", romIsLoaded() ? "" : " not");
	msg("\n");
}

uint8_t 
VC1541Memory::peek(u16 addr)
{
    if (addr >= 0x8000) {
        
        // 0xC000 - 0xFFFF : ROM
        // 0x8000 - 0xBFFF : ROM (repeated)
        //return mem[addr | 0xC000];
        return rom[addr & 0x3FFF];
        
    } else {
        
        // Map to range 0x0000 - 0x1FFF
        addr &= 0x1FFF;
        
        // 0x0000 - 0x07FF : RAM
        // 0x0800 - 0x17FF : unmapped
        // 0x1800 - 0x1BFF : VIA 1 (repeats every 16 bytes)
        // 0x1C00 - 0x1FFF : VIA 2 (repeats every 16 bytes)
        return
        (addr < 0x0800) ? ram[addr] :
        (addr < 0x1800) ? addr >> 8 :
        (addr < 0x1C00) ? drive->via1.peek(addr & 0xF) :
        drive->via2.peek(addr & 0xF);
    }
}

uint8_t
VC1541Memory::spypeek(u16 addr)
{
    if (addr >= 0x8000) {
        return rom[addr & 0x3FFF];
    } else {
        addr &= 0x1FFF;
        return
        (addr < 0x0800) ? ram[addr] :
        (addr < 0x1800) ? addr >> 8 :
        (addr < 0x1C00) ? drive->via1.spypeek(addr & 0xF) :
        drive->via2.spypeek(addr & 0xF);
    }
}

void 
VC1541Memory::poke(u16 addr, uint8_t value)
{
    if (addr >= 0x8000) { // ROM
        return;
    }
    
    // Map to range 0x0000 - 0x1FFF
    addr &= 0x1FFF;
    
    if (addr < 0x0800) { // RAM
        ram[addr] = value;
        return;
    }
    
    if (addr >= 0x1C00) { // VIA 2
        drive->via2.poke(addr & 0xF, value);
        return;
    }
    
    if (addr >= 0x1800) { // VIA 1
        drive->via1.poke(addr & 0xF, value);
        return;
    }
}

