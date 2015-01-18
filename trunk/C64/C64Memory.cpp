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
	
	basicRomIsVisible = false;
	kernelRomIsVisible = false;
	charRomIsVisible = false;
	IOIsVisible = false;
	cartridgeRomIsVisible = false;
	initializePeekPokeLookupTables();
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
    
	// Zero out RAM...
	for (unsigned i = 0; i < sizeof(ram); i++)
        ram[i] = 0;
	
	// Initialize color memory...
	// It's important here to write in random values as some games peek the color RAM 
	// to generate random numbers.
    for (unsigned i = 0; i < sizeof(colorRam); i++) {
        colorRam[i] = (rand() & 0xFF);
        // fprintf(stderr, "%2X ", colorRam[i]);
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
	
	basicRomIsVisible = (bool)read8(buffer);
	charRomIsVisible = (bool)read8(buffer);
	kernelRomIsVisible = (bool)read8(buffer);
	IOIsVisible = (bool)read8(buffer);
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
	
	write8(buffer, (uint8_t)basicRomIsVisible);
	write8(buffer, (uint8_t)charRomIsVisible);
	write8(buffer, (uint8_t)kernelRomIsVisible);
	write8(buffer, (uint8_t)IOIsVisible);
	write8(buffer, (uint8_t)cartridgeRomIsVisible);
}

void 
C64Memory::dumpState()
{
	msg("C64 Memory:\n");
	msg("-----------\n");
	msg("    Basic ROM :%s loaded,%s visible\n", basicRomIsLoaded() ? "" : " not", basicRomIsVisible ? "" : " not");
	msg("Character ROM :%s loaded,%s visible\n", charRomIsLoaded() ? "" : " not", charRomIsVisible ? "" : " not");
	msg("   Kernel ROM :%s loaded,%s visible\n", kernelRomIsLoaded() ? "" : " not", kernelRomIsVisible ? "" : " not");
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

uint8_t 
C64Memory::peekCartridge(uint16_t addr)
{
	if (cartridge != NULL && cartridge->isRomAddr(addr))
		return cartridge->peek(addr);
	
	if (addr >= 0xA000 && addr <= 0xBFFF) 
		return basicRomIsVisible ? rom[addr] : ram[addr];

	if (addr >= 0xE000) 
		return kernelRomIsVisible ? rom[addr] : ram[addr];

	if (IOIsVisible && addr >= 0xDE00 && addr <= 0xDFFF)
		return (uint8_t)(rand());
	
	return ram[addr];
}

void 
C64Memory::initializePeekPokeLookupTables()
{	
	for (unsigned i = 0x1; i <= 0xF; i++) 
		peekSource[i] = MEM_SOURCE_RAM;
	peekSource[0x0] = MEM_SOURCE_PP;

	for (unsigned i = 0x1; i <= 0xF; i++)
		pokeTarget[i] = MEM_SOURCE_RAM;
	pokeTarget[0x0] = MEM_SOURCE_PP;
}

void 
C64Memory::updatePeekPokeLookupTables()
{
    // Addr    LHGE   LHGE   LHGE   LHGE   LHGE   LHGE   LHGE   LHGE   LHGE
    // Range   1111   101X   1000   011X   001X   1110   0100   1100   XX01
    //       default                00X0                             Ultimax
    // -------------------------------------------------------------------------
    // E000-FFFF Kernal  RAM    RAM   Kernal  RAM   Kernal Kernal Kernal ROMH(*)
    // D000-DFFF IO/CHR IO/CHR IO/RAM IO/CHR  RAM   IO/CHR IO/CHR IO/CHR   I/O
    // C000-CFFF  RAM    RAM    RAM    RAM    RAM    RAM    RAM    RAM     -
    // A000-BFFF BASIC   RAM    RAM    RAM    RAM   BASIC   ROMH   ROMH    -
    // 8000-9FFF  RAM    RAM    RAM    RAM    RAM    ROML   RAM    ROML  ROML(*)
    // 4000-7FFF  RAM    RAM    RAM    RAM    RAM    RAM    RAM    RAM     -
    // 1000-3FFF  RAM    RAM    RAM    RAM    RAM    RAM    RAM    RAM     -
    // 0000-0FFF  RAM    RAM    RAM    RAM    RAM    RAM    RAM    RAM    RAM
    //
    // (*) Internal memory does not respond to write accesses in these areas

    // TODO: IMPLEMENT PROPER CRT ROM SELECTION
    
	MemorySource source;
	
	// 0x8000 - 0x9FFFF (Cartridge ROM, or RAM)
	source = cartridge ? MEM_SOURCE_CRT : MEM_SOURCE_RAM;
	peekSource[0x8] = source;
	peekSource[0x9] = source;
		
	// 0xA000 - 0xBFFF (Cartridge ROM, Basic ROM, or RAM)
	source = cartridge ? MEM_SOURCE_CRT : (basicRomIsVisible ? MEM_SOURCE_ROM : MEM_SOURCE_RAM);
	peekSource[0xA] = source;
	peekSource[0xB] = source;
	
	// 0xD000 - 0xDFFF (IO space, Character ROM, or RAM)	
	source = IOIsVisible ? MEM_SOURCE_IO : (charRomIsVisible ? MEM_SOURCE_ROM : MEM_SOURCE_RAM);
	peekSource[0xD] = source;
	
	// 0xE000 - 0xFFFF (Cartridge Rom, Kernel ROM, or RAM)
	source = cartridge ? MEM_SOURCE_CRT : (kernelRomIsVisible ? MEM_SOURCE_ROM : MEM_SOURCE_RAM);
	peekSource[0xE] = source;
	peekSource[0xF] = source;	

	
	MemorySource target;

	// 0xD000 - 0xDFFF (IO space, Character ROM, or RAM)	
	target = IOIsVisible ? MEM_SOURCE_IO : MEM_SOURCE_RAM;
	pokeTarget[0xD] = target;
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
	MemorySource source = peekSource[addr >> 12];
	
	switch(source) {

		case MEM_SOURCE_RAM:
			return ram[addr];
			
		case MEM_SOURCE_ROM:
			return rom[addr];
			
		case MEM_SOURCE_IO:
			return peekIO(addr);
			
		case MEM_SOURCE_CRT:
			return peekCartridge(addr);
						
		case MEM_SOURCE_PP:
			
			uint8_t dir, ext; 
			
			if (addr > 0x0001)
				return ram[addr];

			// Processor port
			dir = cpu->getPortDirection();
			ext = cpu->getExternalPortBits();
			return (addr == 0x0000) ? dir : (dir & cpu->getPort()) | (~dir & ext);
			
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
	if (cartridge != NULL && cartridge->isRomAddr(addr)) {
		cartridge->poke(addr, value);
	} else {
		rom[addr] = value;
	}
}

void
C64Memory::processorPortHasChanged(uint8_t newPortLines)
{
	// Processor port.
	// Bits #0-#2: Configuration for memory areas $A000-$BFFF, $D000-$DFFF and $E000-$FFFF. Values:
	//	           x00:  RAM visible in all three areas.
	//  		   x01:  RAM visible at $A000-$BFFF and $E000-$FFFF.
	// 			   x10: RAM visible at $A000-$BFFF; KERNAL ROM visible at $E000-$FFFF.
	//			   x11: BASIC ROM visible at $A000-$BFFF; KERNAL ROM visible at $E000-$FFFF.
	//			   0xx: Character ROM visible at $D000-$DFFF. (Except for the value %000, see above.)
	//			   1xx: I/O area visible at $D000-$DFFF. (Except for the value %100, see above.)
	//
	// Bit #3:     Datasette output signal level.
	// Bit #4:     Datasette button status; 0 = One or more of PLAY, RECORD, F.FWD or REW pressed; 1 = No button is pressed.
	// Bit #5:     Datasette motor control; 0 = On; 1 = Off.
	
	//          $01   $a000-$bfff  $d000-$dfff  $e000-$ffff
	//	*     -----------------------------------------------
	//	*      0 000      RAM          RAM          RAM
	//	*      1 001      RAM       Char ROM        RAM
	//	*      2 010      RAM       Char ROM    Kernal ROM
	//	*      3 011   Basic ROM    Char ROM    Kernal ROM
	//	*      4 100      RAM          RAM          RAM
	//	*      5 101      RAM          I/O          RAM
	//	*      6 110      RAM          I/O      Kernal ROM
	//	*      7 111   Basic ROM       I/O      Kernal ROM
	
	kernelRomIsVisible = ((newPortLines & 2) == 2); // x1x
	basicRomIsVisible  = ((newPortLines & 3) == 3); // x11
	charRomIsVisible   = ((newPortLines & 4) == 0) && ((newPortLines & 3) != 0); // 0xx, but not x00
	IOIsVisible        = ((newPortLines & 4) == 4) && ((newPortLines & 3) != 0); // 1xx, but not x00 		

	updatePeekPokeLookupTables();
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
		// Expansion port I/O.
		if (cartridge != NULL && cartridgeRomIsVisible) {
			cartridge->poke(addr, value);
			// store for debugging purposes (DE00-DFFF are I/O or RAM addresses)
			rom[addr] = value;
		}
		return;
	}

	assert(false);
}

void C64Memory::poke(uint16_t addr, uint8_t value)
{	

	MemorySource target = pokeTarget[addr >> 12];
	
	switch(target) {
			
		case MEM_SOURCE_RAM:
			ram[addr] = value;
			return;
			
		case MEM_SOURCE_IO:
			pokeIO(addr, value);
			return;
			
		case MEM_SOURCE_PP:
			
			if (addr > 0x0001) {
				ram[addr] = value;
				return;
			}
			
			// Processor port
			if (addr == 0x0000) {
				cpu->setPortDirection(value);
				processorPortHasChanged(cpu->getPortLines());
			} else {
				cpu->setPort(value);
				processorPortHasChanged(cpu->getPortLines());
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



