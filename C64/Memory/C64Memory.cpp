/*!
 * @file        C64Memory.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
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

        { ram,             sizeof(ram),            KEEP_ON_RESET },
        { colorRam,        sizeof(colorRam),       KEEP_ON_RESET },
        { &rom[0xA000],    0x2000,                 KEEP_ON_RESET }, /* Basic ROM */
        { &rom[0xD000],    0x1000,                 KEEP_ON_RESET }, /* Character ROM */
        { &rom[0xE000],    0x2000,                 KEEP_ON_RESET }, /* Kernal ROM */
        { &ramInitPattern, sizeof(ramInitPattern), KEEP_ON_RESET },
        { &peekSrc,        sizeof(peekSrc),        KEEP_ON_RESET },
        { &pokeTarget,     sizeof(pokeTarget),     KEEP_ON_RESET },
        { NULL,            0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
    ramInitPattern = INIT_PATTERN_C64;
    
    //
    // Setup the C64's memory bank map
    //
    
    // If x = (EXROM, GAME, CHAREN, HIRAM, LORAM), then
    //   map[x][0] = mapping for range $1000 - $7FFF
    //   map[x][1] = mapping for range $8000 - $9FFF
    //   map[x][2] = mapping for range $A000 - $BFFF
    //   map[x][3] = mapping for range $C000 - $CFFF
    //   map[x][4] = mapping for range $D000 - $DFFF
    //   map[x][5] = mapping for range $E000 - $FFFF
    MemoryType map[32][6] = {
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_CRTHI, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_CRTLO, M_CRTHI, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_CRTHI, M_RAM,  M_IO,   M_KERNAL},
        {M_RAM,  M_CRTLO, M_CRTHI, M_RAM,  M_IO,   M_KERNAL},
        
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_CRTLO, M_BASIC, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_KERNAL},
        {M_RAM,  M_CRTLO, M_BASIC, M_RAM,  M_IO,   M_KERNAL},
        
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        {M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI},
        
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_BASIC, M_RAM,  M_CHAR, M_KERNAL},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM},
        {M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_KERNAL},
        {M_RAM,  M_RAM,   M_BASIC, M_RAM,  M_IO,   M_KERNAL}
    };
    
    for (unsigned i = 0; i < 32; i++) {
        bankMap[i][0x0] = M_PP;
        bankMap[i][0x1] = map[i][0];
        bankMap[i][0x2] = map[i][0];
        bankMap[i][0x3] = map[i][0];
        bankMap[i][0x4] = map[i][0];
        bankMap[i][0x5] = map[i][0];
        bankMap[i][0x6] = map[i][0];
        bankMap[i][0x7] = map[i][0];
        bankMap[i][0x8] = map[i][1];
        bankMap[i][0x9] = map[i][1];
        bankMap[i][0xA] = map[i][2];
        bankMap[i][0xB] = map[i][2];
        bankMap[i][0xC] = map[i][3];
        bankMap[i][0xD] = map[i][4];
        bankMap[i][0xE] = map[i][5];
        bankMap[i][0xF] = map[i][5];
    }
}

C64Memory::~C64Memory()
{
	debug(3, "  Releasing main memory at address %p...\n", this);
}

void
C64Memory::reset()
{
    VirtualComponent::reset();
    
    // Erase RAM
    eraseWithPattern(ramInitPattern);
        
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
C64Memory::dump()
{
	msg("C64 Memory:\n");
	msg("-----------\n");
	msg("    Basic ROM: %s loaded\n", basicRomIsLoaded() ? "" : " not");
	msg("Character ROM: %s loaded\n", characterRomIsLoaded() ? "" : " not");
    msg("   Kernal ROM: %s loaded\n", kernalRomIsLoaded() ? "" : " not");
	
    for (uint16_t addr = 0; addr < 0xFFFF; addr++) {
        if (c64->cpu.hardBreakpoint(addr))
			msg("Hard breakpoint at %04X\n", addr);
        if (c64->cpu.softBreakpoint(addr))
            msg("Soft breakpoint at %04X\n", addr);
	}
	msg("\n");
    
    /*
    for (uint16_t addr = 0x1000; addr < 0xB000; addr += 0x400) {
        msg("%04X: ", addr);
        for (unsigned i = 0; i < 30; i++) {
            msg("%02X ", ram[addr + i]);
        }
        msg("\n");
    }
    */
}

void
C64Memory::eraseWithPattern(RamInitPattern pattern)
{
    if (!isRamInitPattern(pattern)) {
        warn("Unknown RAM init pattern. Assuming INIT_PATTERN_C64\n");
        pattern = INIT_PATTERN_C64;
    }
    
    if (pattern == INIT_PATTERN_C64) {
        for (unsigned i = 0; i < sizeof(ram); i++)
            ram[i] = (i & 0x40) ? 0xFF : 0x00;
    } else {
        for (unsigned i = 0; i < sizeof(ram); i++)
            ram[i] = (i & 0x80) ? 0x00 : 0xFF;
    }
    
    // Make the screen look nice on startup
    memset(&ram[0x400], 0x01, 40*25);
}

void 
C64Memory::updatePeekPokeLookupTables()
{
    // debug("C64Memory::updatePeekPokeLookupTables\n");
    
    // Read game line, exrom line, and processor port bits
    uint8_t game  = c64->expansionport.getGameLinePhi2() ? 0x08 : 0x00;
    uint8_t exrom = c64->expansionport.getExromLinePhi2() ? 0x10 : 0x00;
    uint8_t index = (c64->processorPort.read() & 0x07) | exrom | game;

    // Set ultimax flag
    c64->setUltimax(exrom && !game);

    // Update table entries
    for (unsigned bank = 1; bank < 16; bank++) {
        peekSrc[bank] = pokeTarget[bank] = bankMap[index][bank];
    }
    
    // An attached cartridge may influence the settings. Let's give it a chance
    // to modify the tables...
    c64->expansionport.updatePeekPokeLookupTables();
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
        return c64->vic.getDataBusPhi1();
        
        default:
        assert(0);
        return 0;
    }
}

uint8_t
C64Memory::peek(uint16_t addr, bool gameLine, bool exromLine)
{
    uint8_t game  = gameLine ? 0x08 : 0x00;
    uint8_t exrom = exromLine ? 0x10 : 0x00;
    uint8_t index = (c64->processorPort.read() & 0x07) | exrom | game;
    
    return peek(addr, bankMap[index][addr >> 12]);
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
 
            return
            (colorRam[addr - 0xD800] & 0x0F) |
            (c64->vic.getDataBusPhi1() & 0xF0);

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

uint8_t
C64Memory::spypeekIO(uint16_t addr)
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

void
C64Memory::poke(uint16_t addr, uint8_t value, MemoryType target)
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

void
C64Memory::poke(uint16_t addr, uint8_t value, bool gameLine, bool exromLine)
{
    uint8_t game  = gameLine ? 0x08 : 0x00;
    uint8_t exrom = exromLine ? 0x10 : 0x00;
    uint8_t index = (c64->processorPort.read() & 0x07) | exrom | game;
    
    poke(addr, value, bankMap[index][addr >> 12]);
}

void
C64Memory::pokeZP(uint8_t addr, uint8_t value)
{
    if (likely(addr >= 0x02)) {
        ram[addr] = value;
    } else if (addr == 0x00) {
        c64->processorPort.writeDirection(value);
    } else {
        c64->processorPort.write(value);
    }
}

void
C64Memory::pokeIO(uint16_t addr, uint8_t value)
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

uint16_t
C64Memory::nmiVector() {
    
    if (peekSrc[0xF] != M_ROM || kernalRomIsLoaded()) {
        return LO_HI(peek(0xFFFA), peek(0xFFFB));
    } else {
        return 0xFE43;
    }
}

uint16_t
C64Memory::irqVector() {
    
    if (peekSrc[0xF] != M_ROM || kernalRomIsLoaded()) {
        return LO_HI(peek(0xFFFE), peek(0xFFFF));
    } else {
        return 0xFF48;
    }
}

uint16_t
C64Memory::resetVector() {
    
    if (peekSrc[0xF] != M_ROM || kernalRomIsLoaded()) {
        debug("Grabbing reset vector from source %d\n", peekSrc[0xF]);
        return LO_HI(peek(0xFFFC), peek(0xFFFD));
    } else {
        return 0xFCE2;
    }
}

