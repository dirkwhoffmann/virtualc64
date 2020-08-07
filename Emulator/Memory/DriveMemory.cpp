// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

DriveMemory::DriveMemory(C64 &ref, Drive &dref) : Memory(ref), drive(dref)
{
    setDescription("1541MEM");

    // this->drive = drive;
    
    memset(rom, 0, sizeof(rom));
    stack = &ram[0x0100];
    
    // Register snapshot items
    SnapshotItem items[] = {

    { ram,  sizeof(ram), KEEP_ON_RESET },
    { rom,  sizeof(rom), KEEP_ON_RESET },
    { NULL, 0,           0 }};

    registerSnapshotItems(items, sizeof(items));
}

void 
DriveMemory::_reset()
{
    // Clear snapshot items marked with 'CLEAR_ON_RESET'
     if (snapshotItems != NULL)
         for (unsigned i = 0; snapshotItems[i].data != NULL; i++)
             if (snapshotItems[i].flags & CLEAR_ON_RESET)
                 memset(snapshotItems[i].data, 0, snapshotItems[i].size);

    // Initialize RAM with powerup pattern (pattern from Hoxs64)
    for (unsigned i = 0; i < sizeof(ram); i++) {
        ram[i] = (i & 64) ? 0xFF : 0x00;
    }
}

void 
DriveMemory::_dump()
{
	msg("VC1541 Memory:\n");
	msg("--------------\n\n");
    msg("VC1541 ROM :%s loaded\n", c64.hasVC1541Rom() ? "" : " not");
	msg("\n");
}

u8 
DriveMemory::peek(u16 addr)
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
        (addr < 0x1C00) ? drive.via1.peek(addr & 0xF) :
        drive.via2.peek(addr & 0xF);
    }
}

u8
DriveMemory::spypeek(u16 addr)
{
    if (addr >= 0x8000) {
        return rom[addr & 0x3FFF];
    } else {
        addr &= 0x1FFF;
        return
        (addr < 0x0800) ? ram[addr] :
        (addr < 0x1800) ? addr >> 8 :
        (addr < 0x1C00) ? drive.via1.spypeek(addr & 0xF) :
        drive.via2.spypeek(addr & 0xF);
    }
}

void 
DriveMemory::poke(u16 addr, u8 value)
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
        drive.via2.poke(addr & 0xF, value);
        return;
    }
    
    if (addr >= 0x1800) { // VIA 1
        drive.via1.poke(addr & 0xF, value);
        return;
    }
}

