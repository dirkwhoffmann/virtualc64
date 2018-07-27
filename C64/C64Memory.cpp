/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2008 - 2018 Dirk W. Hoffmann
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

C64Memory::C64Memory()
{	
	setDescription("C64 memory");
    
	debug (3, "  Creating main memory at address %p...\n", this);
		
    memset(rom, 0, sizeof(rom));
    stack = &ram[0x0100];
    
    // Register snapshot items
    SnapshotItem items[] = {

        { ram,          sizeof(ram),        KEEP_ON_RESET },
        { colorRam,     sizeof(colorRam),   KEEP_ON_RESET },
        { &rom[0xA000], 0x2000,             KEEP_ON_RESET  }, /* Basic ROM */
        { &rom[0xD000], 0x1000,             KEEP_ON_RESET  }, /* Character ROM */
        { &rom[0xE000], 0x2000,             KEEP_ON_RESET  }, /* Kernal ROM */
        { &peekSrc,     sizeof(peekSrc),    KEEP_ON_RESET },
        { &pokeTarget,  sizeof(pokeTarget), KEEP_ON_RESET },
        { NULL,         0,                  0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

C64Memory::~C64Memory()
{
	debug(3, "  Releasing main memory at address %p...\n", this);
}

void
C64Memory::reset()
{
    bool newc64 = false;
    
    VirtualComponent::reset();
    
    // Initialize RAM with powerup pattern
    if (newc64) {
        for (unsigned i = 0; i < sizeof(ram); i++)
            ram[i] = (i & 0x80) ? 0x00 : 0xFF;
    } else {
        for (unsigned i = 0; i < sizeof(ram); i++)
            ram[i] = (i & 0x40) ? 0xFF : 0x00;
    }
    
    // Clear out initially visible screen memory to make it look nicer on startup
    for (unsigned i = 0; i < 1000; i++)
        ram[0x400+i] = 0x00;
    
    // Initialize color RAM with random numbers
    srand(1000);
    for (unsigned i = 0; i < sizeof(colorRam); i++) {
        colorRam[i] = (rand() & 0xFF);
    }
    
    // Initialize peek source lookup table
    for (unsigned i = 0x1; i <= 0xF; i++) {
        peekSrc[i] = M_RAM;
    }
    peekSrc[0x0] = M_PP;
    
    // Initialize poke source lookup table
    for (unsigned i = 0x1; i <= 0xF; i++) {
        pokeTarget[i] = M_RAM;
    }
    pokeTarget[0x0] = M_PP;
}

void 
C64Memory::dumpState()
{
	msg("C64 Memory:\n");
	msg("-----------\n");
	msg("    Basic ROM :%s loaded\n", basicRomIsLoaded() ? "" : " not");
	msg("Character ROM :%s loaded\n", charRomIsLoaded() ? "" : " not");
	msg("   Kernal ROM :%s loaded\n", kernalRomIsLoaded() ? "" : " not");
	
    for (uint16_t addr = 0; addr < 0xFFFF; addr++) {
        if (c64->cpu.hardBreakpoint(addr))
			msg("Hard breakpoint at %04X\n", addr);
        if (c64->cpu.softBreakpoint(addr))
            msg("Soft breakpoint at %04X\n", addr);
	}
	msg("\n");
}


//
// Accessing the memory
//

void 
C64Memory::updatePeekPokeLookupTables()
{
    MemoryType source;
    // MemoryType target;
    
    uint8_t exrom = c64->expansionport.getExromLine() ? 0x10 : 0x00;
    uint8_t game  = c64->expansionport.getGameLine() ? 0x08 : 0x00;
    uint8_t index = (c64->processorPort.read() & 0x07) | exrom | game;

    // Set ultimax flag
    c64->setUltimax(exrom && !game);

    // Set peek sources
    source = BankMap[index][0]; // 0x1000 - 0x7FFF (RAM or open)
    assert(source == M_RAM || source == M_NONE);
    peekSrc[0x1] = pokeTarget[0x1] = source;
    peekSrc[0x2] = pokeTarget[0x2] = source;
    peekSrc[0x3] = pokeTarget[0x3] = source;
    peekSrc[0x4] = pokeTarget[0x4] = source;
    peekSrc[0x5] = pokeTarget[0x5] = source;
    peekSrc[0x6] = pokeTarget[0x6] = source;
    peekSrc[0x7] = pokeTarget[0x7] = source;

    source = BankMap[index][1]; // 0x8000 - 0x9FFF (CRT or RAM)
    assert(source == M_CRTLO || source == M_RAM);
    peekSrc[0x8] = pokeTarget[0x8] = source;
    peekSrc[0x9] = pokeTarget[0x9] = source;

    source = BankMap[index][2]; // 0xA000 - 0xBFFF (CRT, Basic ROM, RAM, or open)
    assert(source == M_CRTHI || source == M_BASIC ||
           source == M_RAM   || source == M_NONE);
    peekSrc[0xA] = pokeTarget[0xA] = source;
    peekSrc[0xB] = pokeTarget[0xB] = source;

    source = BankMap[index][3]; // 0xC000 - 0xCFFF (RAM or open)
    assert(source == M_RAM || source == M_NONE);
    peekSrc[0xC] = pokeTarget[0xC] = source;

    source = BankMap[index][4]; // 0xD000 - 0xDFFF (I/O, Character ROM, or RAM)
    assert(source == M_IO || source == M_CHAR || source == M_RAM);
    peekSrc[0xD] = pokeTarget[0xD] = source;

    source = BankMap[index][5]; // 0xE000 - 0xFFFF (CRT, Kernal ROM, or RAM)
    assert(source == M_CRTHI || source == M_KERNAL || source == M_RAM);
    peekSrc[0xE] = pokeTarget[0xE] = source;
    peekSrc[0xF] = pokeTarget[0xF] = source;

    /*
    // Set poke targets
    target = BankMap[index][1]; // 0x8000 - 0x9FFF (CRT or RAM)
    if (target != M_CRTLO) target = M_RAM;
    pokeTarget[0x8] = target;
    pokeTarget[0x9] = target;
    
    target = BankMap[index][2]; // 0xA000 - 0xBFFF (CRT, or RAM)
    if (target != M_CRTHI) target = M_RAM;
    pokeTarget[0xA] = target;
    pokeTarget[0xB] = target;
    
    target = BankMap[index][4]; // 0xD000 - 0xDFFF (I/O or RAM)
    if (target != M_IO) target = M_RAM;
    pokeTarget[0xD] = target;
    
    target = BankMap[index][5]; // 0xE000 - 0xFFFF (CRT, or RAM)
    if (target != M_CRTHI) target = M_RAM;
    pokeTarget[0xE] = target;
    pokeTarget[0xF] = target;
    */
}

uint8_t
C64Memory::peek(uint16_t addr, MemoryType source)
{
    switch(source) {
            
        case M_RAM:
            return ram[addr];
            
        case M_ROM:
            return rom[addr];
            
        case M_IO:
            return peekIO(addr);
            
        case M_CRTLO:
        case M_CRTHI:
            return c64->expansionport.peek(addr);
            
        case M_PP:
            if (likely(addr >= 0x02)) {
                return ram[addr];
            } else if (addr == 0x00) {
                return c64->processorPort.readDirection();
            } else {
                return c64->processorPort.read();
            }
   
        case M_NONE:
            // what happens if RAM is unmapped?
            return ram[addr];
            
        default:
            assert(0);
            return 0;
    }
}

uint8_t
C64Memory::peekZP(uint8_t addr)
{
    if (likely(addr >= 0x02)) {
        return ram[addr];
    } else if (addr == 0x00) {
        return c64->processorPort.readDirection();
    } else {
        return c64->processorPort.read();
    }
}

uint8_t
C64Memory::peekIO(uint16_t addr)
{
    assert(addr >= 0xD000 && addr <= 0xDFFF);
    
    switch ((addr >> 8) & 0xF) {
            
        case 0x0: // VIC
        case 0x1: // VIC
        case 0x2: // VIC
        case 0x3: // VIC
            
            // Only the lower 6 bits are used for adressing the VIC I/O space.
            // As a result, VIC's I/O memory repeats every 64 bytes.
            return c64->vic.peek(addr & 0x003F);

        case 0x4: // SID
        case 0x5: // SID
        case 0x6: // SID
        case 0x7: // SID
            
            // Only the lower 5 bits are used for adressing the SID I/O space.
            // As a result, SID's I/O memory repeats every 32 bytes.
            return c64->sid.peek(addr & 0x001F);

        case 0x8: // Color RAM
        case 0x9: // Color RAM
        case 0xA: // Color RAM
        case 0xB: // Color RAM
 
            return (colorRam[addr - 0xD800] & 0x0F) | (c64->vic.dataBus & 0xF0);

        case 0xC: // CIA 1
 
            // Only the lower 4 bits are used for adressing the CIA I/O space.
            // As a result, CIA's I/O memory repeats every 16 bytes.
            return c64->cia1.peek(addr & 0x000F);
	
        case 0xD: // CIA 2
            
            return c64->cia2.peek(addr & 0x000F);
            
        case 0xE: // I/O space 1
            
            return c64->expansionport.peekIO1(addr);
            
        case 0xF: // I/O space 2

            return c64->expansionport.peekIO2(addr);
	}
    
	assert(false);
	return 0;
}

uint8_t
C64Memory::spypeek(uint16_t addr, MemoryType source)
{
    switch(source) {
            
        case M_RAM:
            return ram[addr];
            
        case M_ROM:
            return rom[addr];
            
        case M_IO:
            return spypeekIO(addr);
            
        case M_CRTLO:
        case M_CRTHI:
            return c64->expansionport.spypeek(addr);
            
        case M_PP:
            return peek(addr, M_PP);
      
        case M_NONE:
            return ram[addr];
            
        default:
            assert(0);
            return 0;
    }
}

uint8_t C64Memory::spypeekIO(uint16_t addr)
{
    assert(addr >= 0xD000 && addr <= 0xDFFF);
    
    switch ((addr >> 8) & 0xF) {
            
        case 0x0: // VIC
        case 0x1: // VIC
        case 0x2: // VIC
        case 0x3: // VIC
            
            return c64->vic.spypeek(addr & 0x003F);
            
        case 0x4: // SID
        case 0x5: // SID
        case 0x6: // SID
        case 0x7: // SID
            
            return c64->sid.spypeek(addr & 0x001F);
            
        case 0xC: // CIA 1
            
            // Only the lower 4 bits are used for adressing the CIA I/O space.
            // As a result, CIA's I/O memory repeats every 16 bytes.
            return c64->cia1.spypeek(addr & 0x000F);
            
        case 0xD: // CIA 2
            
            return c64->cia2.spypeek(addr & 0x000F);
            
        case 0xE: // I/O space 1
            
            return c64->expansionport.spypeekIO1(addr);
            
        case 0xF: // I/O space 2
            
            return c64->expansionport.spypeekIO2(addr);

        default:
            
            return peek(addr);
    }
}

void C64Memory::poke(uint16_t addr, uint8_t value, MemoryType target)
{
    switch(target) {
            
        case M_RAM:
        case M_ROM:
            ram[addr] = value;
            return;
            
        case M_IO:
            pokeIO(addr, value);
            return;
            
        case M_CRTLO:
        case M_CRTHI:
            c64->expansionport.poke(addr, value);
            return;
            
        case M_PP:
            if (likely(addr >= 0x02)) {
                ram[addr] = value;
            } else if (addr == 0x00) {
                c64->processorPort.writeDirection(value);
            } else {
                c64->processorPort.write(value);
            }
            return;
            
        case M_NONE:
            return;
            
        default:
            assert(0);
            return;
    }
}

void C64Memory::pokeZP(uint8_t addr, uint8_t value)
{
    if (likely(addr >= 0x02)) {
        ram[addr] = value;
    } else if (addr == 0x00) {
        c64->processorPort.writeDirection(value);
    } else {
        c64->processorPort.write(value);
    }
}

void C64Memory::pokeIO(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xD000 && addr <= 0xDFFF);
    
    switch ((addr >> 8) & 0xF) {
            
        case 0x0: // VIC
        case 0x1: // VIC
        case 0x2: // VIC
        case 0x3: // VIC
            
            // Only the lower 6 bits are used for adressing the VIC I/O space.
            // As a result, VIC's I/O memory repeats every 64 bytes.
            c64->vic.poke(addr & 0x003F, value);
            return;
            
        case 0x4: // SID
        case 0x5: // SID
        case 0x6: // SID
        case 0x7: // SID
            
            // Only the lower 5 bits are used for adressing the SID I/O space.
            // As a result, SID's I/O memory repeats every 32 bytes.
            c64->sid.poke(addr & 0x001F, value);
            return;
            
        case 0x8: // Color RAM
        case 0x9: // Color RAM
        case 0xA: // Color RAM
        case 0xB: // Color RAM
            
            colorRam[addr - 0xD800] = (value & 0x0F) | (rand() & 0xF0);
            return;
            
        case 0xC: // CIA 1
            
            // Only the lower 4 bits are used for adressing the CIA I/O space.
            // As a result, CIA's I/O memory repeats every 16 bytes.
            c64->cia1.poke(addr & 0x000F, value);
            return;
            
        case 0xD: // CIA 2
            
            c64->cia2.poke(addr & 0x000F, value);
            return;
            
        case 0xE: // I/O space 1
            
            c64->expansionport.pokeIO1(addr, value);
            return;
            
        case 0xF: // I/O space 2
            
            c64->expansionport.pokeIO2(addr, value);
            return;
    }
    
    assert(false);
}
