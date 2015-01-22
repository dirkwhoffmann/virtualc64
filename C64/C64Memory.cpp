/*
 * (C) 2008 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
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

// --------------------------------------------------------------------------------
//                          Construction and Destruction
// --------------------------------------------------------------------------------

C64Memory::C64Memory(C64 *c64)
{	
	name ="C64 memory";
	
	debug (2, "  Creating main memory at address %p...\n", this);
	
    this->c64 = c64;
	
	cartridge = NULL;	

	charRomFile = NULL;
	kernelRomFile = NULL;
	basicRomFile = NULL;
	
	cartridgeRomIsVisible = false;
}

C64Memory::~C64Memory()
{
	debug(2, "  Releasing main memory at address %p...\n", this);
}

void C64Memory::reset()
{
	debug (2, "  Resetting main memory...\n");
	
    // Establish bindings
    vic = c64->vic;
    sid = c64->sid;
    cia1 = c64->cia1;
    cia2 = c64->cia2;
    cpu = c64->cpu;
    
	// Zero out RAM
	for (unsigned i = 0; i < sizeof(ram); i++)
        ram[i] = 0;
	
    // Initialize peek source lookup table
    for (unsigned i = 0x1; i <= 0xF; i++)
        peekSrc[i] = M_RAM;
    peekSrc[0x0] = M_PP;
    
    // Initialize poke source lookup table
    for (unsigned i = 0x1; i <= 0xF; i++)
        pokeTarget[i] = M_RAM;
    pokeTarget[0x0] = M_PP;

	// Initialize color memory with random numbers
    for (unsigned i = 0; i < sizeof(colorRam); i++) {
        colorRam[i] = (rand() & 0xFF);
    }
    
	// Initialize processor port data direction register and processor port
	poke(0x0000, 0x2F); // Data direction
	poke(0x0001, 0x1F);	// IO port, set default memory layout
}	

// --------------------------------------------------------------------------------
//                                      Input / Output
// --------------------------------------------------------------------------------

void
C64Memory::loadFromBuffer(uint8_t **buffer)
{	
	debug(2, "  Loading C64 memory state...\n");
	
	Memory::loadFromBuffer(buffer);
	
	readBlock(buffer, ram, sizeof(ram)); 
	readBlock(buffer, colorRam, sizeof(colorRam)); 
	readBlock(buffer, &rom[0xA000], 0x2000); // Basic ROM
	readBlock(buffer, &rom[0xD000], 0x1000); // Character ROM
	readBlock(buffer, &rom[0xE000], 0x2000); // Kernel ROM
	
	(void)read8(buffer);
	(void)read8(buffer);
	(void)read8(buffer);
	(void)read8(buffer);
	cartridgeRomIsVisible = (bool)read8(buffer);
	updatePeekPokeLookupTables();
}

void
C64Memory::saveToBuffer(uint8_t **buffer) 
{
	debug(2, "  Saving C64 memory state...\n");
	
	Memory::saveToBuffer(buffer);
	
	writeBlock(buffer, ram, sizeof(ram)); 
	writeBlock(buffer, colorRam, sizeof(colorRam)); 
	writeBlock(buffer, &rom[0xA000], 0x2000); // Basic ROM
	writeBlock(buffer, &rom[0xD000], 0x1000); // Character ROM
	writeBlock(buffer, &rom[0xE000], 0x2000); // Kernel ROM
	
	write8(buffer, (uint8_t)0);
	write8(buffer, (uint8_t)0);
	write8(buffer, (uint8_t)0);
	write8(buffer, (uint8_t)0);
	write8(buffer, (uint8_t)cartridgeRomIsVisible);
}

void 
C64Memory::dumpState()
{
	msg("C64 Memory:\n");
	msg("-----------\n");
	msg("    Basic ROM :%s loaded\n", basicRomIsLoaded() ? "" : " not");
	msg("Character ROM :%s loaded\n", charRomIsLoaded() ? "" : " not");
	msg("   Kernel ROM :%s loaded\n", kernelRomIsLoaded() ? "" : " not");
	for (uint16_t i = 0; i < 0xFFFF; i++) {
		uint8_t tag = cpu->getBreakpointTag(i);
		if (tag != CPU::NO_BREAKPOINT) {
			msg("Breakpoint at %0x4X %s\n", i, tag == CPU::SOFT_BREAKPOINT ? "(soft)" : "");
		}
	}
	msg("\n");
}


// --------------------------------------------------------------------------------
//                                      Rom Handling
// --------------------------------------------------------------------------------

bool C64Memory::isBasicRom(const char *filename)
{
	int magic_bytes[] = { 0x94, 0xE3, 0x7B, EOF };

	if (filename == NULL)
		return false;
	
	if (!checkFileSize(filename, 0x2000, 0x2000))
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
		
	return true;
}

bool C64Memory::isCharRom(const char *filename)
{
	int magic_bytes[] = { 0x3C, 0x66, 0x6E, EOF };

	if (filename == NULL)
		return false;
	
	if (!checkFileSize(filename, 0x1000, 0x1000))
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
		
	return true;
}

bool C64Memory::isKernelRom(const char *filename) 
{
	int magic_bytes[] = { 0x85, 0x56, 0x20, EOF };

	if (filename == NULL)
		return false;
	
	if (!checkFileSize(filename, 0x2000, 0x2000))
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
		
	return true;
}

bool C64Memory::isRom(const char *filename) 
{
	return isBasicRom(filename) || isCharRom(filename) || isKernelRom(filename);
}

bool 
C64Memory::loadBasicRom(const char *filename)
{
	if (isBasicRom(filename)) {
		basicRomFile = strdup(filename);
		flashRom(filename, 0xA000);
		return true;
	}
	return false;
}

bool 
C64Memory::loadCharRom(const char *filename)
{
	if (isCharRom(filename)) {
		charRomFile = strdup(filename);
		flashRom(filename, 0xD000);
		return true;
	}
	return false;
}

bool 
C64Memory::loadKernelRom(const char *filename)
{
	if (isKernelRom(filename)) {
		kernelRomFile = strdup(filename);
		flashRom(filename, 0xE000);
		return true;
	}
	return false;
}


// --------------------------------------------------------------------------------
//                              Memory access methods
// --------------------------------------------------------------------------------

bool C64Memory::isValidAddr(uint16_t addr, MemoryType type)
{
	switch (type) {
		case MEM_RAM: 
			return true;
		case MEM_ROM:
			return isRomAddr(addr);
		case MEM_IO:
			return isCharRomAddr(addr);
		default:
			assert(false);
			return false;
	}
}

uint8_t 
C64Memory::peekRam(uint16_t addr) 
{ 
	return ram[addr];
} 

uint8_t 
C64Memory::peekRom(uint16_t addr) 
{ 
	return rom[addr];
} 

#if 0
uint8_t 
C64Memory::peekCartridge(uint16_t addr)
{
	if (cartridge != NULL && cartridge->isRomAddr(addr))
		return cartridge->peek(addr);
    
	return ram[addr];
}
#endif

void 
C64Memory::updatePeekPokeLookupTables()
{
    uint8_t EXROM = c64->expansionport->getExromLine() ? 0x10 : 0x00;
    uint8_t GAME = c64->expansionport->getGameLine() ? 0x08 : 0x00;
    
    uint8_t index = (cpu->getPortLines() & 0x07) | EXROM | GAME;
    
    MemorySource source;

    // 0x1000 - 0x7FFF (RAM or unmapped)
    source = BankMap[index][0];
    assert(source == M_RAM || source == M_NONE);
    peekSrc[0x1] = source;
    peekSrc[0x2] = source;
    peekSrc[0x3] = source;
    peekSrc[0x4] = source;
    peekSrc[0x5] = source;
    peekSrc[0x6] = source;
    peekSrc[0x7] = source;

    // 0x8000 - 0x9FFF (Cartridge ROM or RAM)
    source = BankMap[index][1];
    assert(source == M_CRTLO || source == M_CRTHI || source == M_RAM);
    peekSrc[0x8] = source;
    peekSrc[0x9] = source;

    // 0xA000 - 0xBFFF (Cartridge ROM, basic ROM, RAM, or unmapped)
    source = BankMap[index][2];
    assert(source == M_CRTLO || source == M_CRTHI || source == M_BASIC || source == M_RAM || source == M_NONE);
    peekSrc[0xA] = source;
    peekSrc[0xB] = source;

    // 0xC000 - 0xCFFF (RAM or unmapped)
    source = BankMap[index][3];
    assert(source == M_RAM || source == M_NONE);
    peekSrc[0xC] = source;

    // 0xD000 - 0xDFFF (IO space, character ROM, or RAM)
    source = BankMap[index][4];
    assert(source == M_IO || source == M_CHAR || source == M_RAM);
    peekSrc[0xD] = source;

    // 0xE000 - 0xFFFF (Cartridge Rom, Kernel ROM, or RAM)
    source = BankMap[index][5];
    assert(source == M_CRTLO || source == M_CRTHI || source == M_KERNEL || source == M_RAM);
    peekSrc[0xE] = source;
    peekSrc[0xF] = source;

    MemorySource target;
    
    // 0xD000 - 0xDFFF (IO space, Character ROM, or RAM)
    target = BankMap[index][4];
    assert(target == M_IO || target == M_CHAR || target == M_RAM);
    pokeTarget[0xD] = (target == M_IO ? M_IO : M_RAM);
}


uint8_t C64Memory::peekIO(uint16_t addr)
{
	// 0xD000 - 0xD3FF (VIC)
	if (addr <= 0xD3FF) {
		// Note: Only the lower 6 bits are used for adressing the VIC I/O space
		// Therefore, the VIC I/O memory repeats every 64 bytes
		return vic->peek(addr & 0x003F);	
	}
	
	// 0xD400 - 0xD7FF (SID)
	if (addr <= 0xD7FF) {
		// Note: Only the lower 5 bits are used for adressing the SID I/O space
		// Therefore, the SID I/O memory repeats every 32 bytes
		return sid->peek(addr & 0x001F);
	}
	
	// 0xD800 - 0xDBFF (Color RAM)
	if (addr <= 0xDBFF) {
        return (colorRam[addr - 0xD800] & 0x0F) | (vic->getDataBus() & 0xF0);
	}
	
	// 0xDC00 - 0xDCFF (CIA 1)
	if (addr <= 0xDCFF) {
		// Note: Only the lower 4 bits are used for adressing the CIA I/O space
		// Therefore, the CIA I/O memory repeats every 16 bytes
		return cia1->peek(addr & 0x000F);
	}
	
	// 0xDD00 - 0xDDFF (CIA 2)
	if (addr <= 0xDDFF) {
		// Note: Only the lower 4 bits are used for adressing the CIA I/O space
		// Therefore, the CIA I/O memory repeats every 16 bytes
		return cia2->peek(addr & 0x000F);
	}
	
	// 0xDE00 - 0xDEFF (I/O area 1)
	// 0xDF00 - 0xDFFF (I/O area 2) 
	if (addr <= 0xDFFF) {

        /* "Die beiden mit "I/O 1" und "I/O 2" bezeichneten Bereiche
            sind für Erweiterungskarten reserviert und normalerweise ebenfalls offen,
            ein Lesezugriff liefert auch hier "zufällige" Daten (dass diese Daten gar
            nicht so zufällig sind, wird in Kapitel 4 noch ausführlich erklärt. Ein
            Lesen von offenen Adressen liefert nämlich auf vielen C64 das zuletzt vom
            VIC gelesene Byte zurück!)" [C.B.] */
        
		return vic->getDataBus();
	}

	assert(false);
	return 0;
}

uint8_t C64Memory::peek(uint16_t addr)
{	
    MemorySource src = peekSrc[addr >> 12];

    switch(src) {
            
        case M_RAM:
            return ram[addr];
            
        case M_ROM:
            return rom[addr];
            
        case M_IO:
            return peekIO(addr);
            
        case M_CRTLO:
        case M_CRTHI:

            if (c64->expansionport->romIsBlendedIn(addr))
                return c64->expansionport->peek(addr);
            else
                return ram[addr];
            
        case M_PP:
            
            uint8_t dir, ext;
            
            if (addr > 0x0001)
                return ram[addr];
            
            // Processor port
            dir = cpu->getPortDirection();
            ext = cpu->getExternalPortBits();
            return (addr == 0x0000) ? dir : (dir & cpu->getPort()) | (~dir & ext);
            
        case M_NONE:
            // what happens if RAM is unmapped?
            return ram[addr];

        default:
            assert(0);
            return 0;
    }
}


// --------------------------------------------------------------------------------
//                                    Poke
// --------------------------------------------------------------------------------

void C64Memory::pokeRam(uint16_t addr, uint8_t value)             
{ 
	ram[addr] = value;
}

void C64Memory::pokeRom(uint16_t addr, uint8_t value)             
{ 
#if 0
    if (cartridge != NULL && cartridge->isRomAddr(addr)) {
        cartridge->poke(addr, value);
    } else {
        rom[addr] = value;
    }
#endif
    
    rom[addr] = value;
}

void C64Memory::pokeIO(uint16_t addr, uint8_t value)
{
	// 0xD000 - 0xD3FF (VIC)
	if (addr < 0xD400) {
		// Note: Only the lower 6 bits are used for adressing the VIC I/O space
		// Therefore, the VIC I/O memory repeats every 64 bytes
		vic->poke(addr & 0x003F, value);
		return;
	}
	
	// 0xD400 - 0xD7FF (SID)
	if (addr < 0xD800) {
		// Note: Only the lower 5 bits are used for adressing the SID I/O space
		// Therefore, the SID I/O memory repeats every 32 bytes
		sid->poke(addr & 0x001F, value);
		return;
	}

	// 0xD800 - 0xDBFF (Color RAM)
	if (addr < 0xDC00) {
		// Note: The color RAM only saves 4 Bit per address (one nibble)
		// When reading the color RAM, the upper 4 bits will contain random values
		colorRam[addr - 0xD800] = (value & 0x0F) | (rand() & 0xF0);
		return;
	}	

	// 0xDC00 - 0xDCFF (CIA 1)
	if (addr < 0xDD00) {
		// Note: Only the lower 4 bits are used for adressing the CIA I/O space
		// Therefore, the VIC I/O memory repeats every 16 bytes
		cia1->poke(addr & 0x000F, value);
		return;
	}
	
	// 0xDD00 - 0xDDFF (CIA 2)	
	if (addr < 0xDE00) {
		// Note: Only the lower 4 bits are used for adressing the CIA I/O space
		// Therefore, the VIC I/O memory repeats every 16 bytes
		cia2->poke(addr & 0x000F, value);
		return;
	}
	
	// 0xDE00 - 0xDEFF (I/O area 1)
	// 0xDF00 - 0xDFFF (I/O area 2)
	if (addr < 0xE000) {
        // Some registers in this area trigger a bank switch in the
        // attached cartridge module. So we pass the value there...
        c64->expansionport->poke(addr, value);
        return;
	}

	assert(false);
}

void C64Memory::poke(uint16_t addr, uint8_t value)
{	

	MemorySource target = pokeTarget[addr >> 12];
	
	switch(target) {
			
		case M_RAM:
			ram[addr] = value;
			return;
			
		case M_IO:
			pokeIO(addr, value);
			return;
			
		case M_PP:
			
			if (addr > 0x0001) {
				ram[addr] = value;
				return;
			}
			
			// Processor port
			if (addr == 0x0000) {
				cpu->setPortDirection(value);
                updatePeekPokeLookupTables();
			} else {
				cpu->setPort(value);
                updatePeekPokeLookupTables();
			}
			return;

		default:
			assert(0);
			return;
	}
}

bool C64Memory::attachCartridge(Cartridge *c)
{
	cartridge = c;
	
	// Cartridge rom is visible when EXROM or GAME lines are pulled low (grounded).
	cartridgeRomIsVisible = c->exromIsHigh()==false || c->gameIsHigh()==false;
	updatePeekPokeLookupTables();
	
	printf ("Cartridge attached %d\n", cartridgeRomIsVisible);
	return true;
}

bool C64Memory::detachCartridge()
{
	cartridge = NULL;	
	cartridgeRomIsVisible = false;
	updatePeekPokeLookupTables();

	return true;
}



