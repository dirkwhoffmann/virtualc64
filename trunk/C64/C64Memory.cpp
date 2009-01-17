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

C64Memory::C64Memory()
{	
	charRomFile = NULL;
	kernelRomFile = NULL;
	basicRomFile = NULL;

	debug ("  Creating main memory at address %p...\n", this);
	
	vic = NULL;
	sid = NULL;
	cia1 = NULL;
	cia2 = NULL;
		
	// Try to auto-load ROMs...
	//(void)loadRom("Char.rom");
	//(void)loadRom("Kernel.rom");
	//(void)loadRom("Basic.rom");
}

C64Memory::~C64Memory()
{
	debug("  Releasing main memory at address %p...\n", this);
}

void C64Memory::reset()
{
	debug ("  Resetting main memory...\n");
	
	// Zero out RAM...
	for (int i = 0; i < sizeof(ram); i++)
		ram[i] = 0;
	
	for (int i = 0; i < sizeof(colorRam); i++)
		colorRam[i] = 0;	
	
	// Initialize processor port data direction register and processor port
	poke(0x0000, 0x2F);
	poke(0x0001, 0x1F);	
}	

// --------------------------------------------------------------------------------
//                                      Input / Output
// --------------------------------------------------------------------------------

bool
C64Memory::load(FILE *file)
{	
	debug("  Loading memory snapshot...\n");
	basicRomIsVisible = (bool)read8(file);
	charRomIsVisible = (bool)read8(file);
	kernelRomIsVisible = (bool)read8(file);
	IOIsVisible = (bool)read8(file);
	
	for (int i=0; i <= 0xffff; i++) {
		ram[i] = read8(file);
	}

	for (int i=0; i < 1024; i++) {
		colorRam[i] = read8(file);
	}
	return true;
}

bool
C64Memory::save(FILE *file) 
{
	debug("  Saving memory snapshot...\n");
	write8(file, (uint8_t)basicRomIsVisible);
	write8(file, (uint8_t)charRomIsVisible);
	write8(file, (uint8_t)kernelRomIsVisible);
	write8(file, (uint8_t)IOIsVisible);
	
	for (int i=0; i<=0xffff; i++) {
		write8(file, ram[i]);
	}
	
	for (int i=0; i < 1024; i++) {
		write8(file, colorRam[i]);
	}
	return true;
}

// --------------------------------------------------------------------------------
//                                      Rom Handling
// --------------------------------------------------------------------------------

bool C64Memory::isBasicRom(const char *filename)
{
	int magic_bytes[] = { 0x94, 0xE3, 0x7B, EOF };

	assert(filename != NULL);
	
	if (!checkFileSize(filename, 0x2000, 0x2000))
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
		
	return true;
}

bool C64Memory::isCharRom(const char *filename)
{
	int magic_bytes[] = { 0x3C, 0x66, 0x6E, EOF };

	assert(filename != NULL);
	
	if (!checkFileSize(filename, 0x1000, 0x1000))
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
		
	return true;
}

bool C64Memory::isKernelRom(const char *filename) 
{
	int magic_bytes[] = { 0x85, 0x56, 0x20, EOF };

	assert(filename != NULL);
	
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
		case MEM_WATCHPOINT:
			return getWatchpointType(addr) != NO_WATCHPOINT;
		default:
			assert(false);
			return false;
	}
}

uint8_t C64Memory::peekRam(uint16_t addr) 
{ 
	return ram[addr]; 
} 

uint8_t C64Memory::peekRom(uint16_t addr) 
{ 
	return rom[addr]; 
} 

uint8_t C64Memory::peekIO(uint16_t addr)
{
	// 0xD000 - 0xD3FF (VIC)
	if (VIC::isVicAddr(addr)) {
		// Note: Only the lower 6 bits are used for adressing the VIC I/O space
		// Therefore, the VIC I/O memory repeats every 64 bytes
		return vic->peek(addr & 0x003F);	
	}
	
	// 0xD400 - 0xD7FF (SID)
	if (SID::isSidAddr(addr)) {
		// Note: Only the lower 5 bits are used for adressing the SID I/O space
		// Therefore, the SID I/O memory repeats every 32 bytes
		return sid->peek(addr & 0x001F);
	}
	
	// 0xD800 - 0xDBFF (Color RAM)
	if (addr >= 0xD800 && addr <= 0xDBFF) {
		return colorRam[addr - 0xD800];
	}
	
	// 0xDC00 - 0xDCFF (CIA 1)
	if (CIA1::isCia1Addr(addr)) {
		// Note: Only the lower 4 bits are used for adressing the CIA I/O space
		// Therefore, the CIA I/O memory repeats every 16 bytes
		return cia1->peek(addr & 0x000F);
	}
	
	// 0xDD00 - 0xDDFF (CIA 2)
	if (CIA2::isCia2Addr(addr)) {
		// Note: Only the lower 4 bits are used for adressing the CIA I/O space
		// Therefore, the CIA I/O memory repeats every 16 bytes
		return cia2->peek(addr & 0x000F);
	}
	
	// 0xDE00 - 0xDEFF (I/O area 1)
	// 0xDF00 - 0xDFFF (I/O area 2) 
	if (addr >= 0xDE00 && addr <= 0xDFFF) {
		// Note: Reserved for further I/O expansion
		// When read, a random value is returned
		return (uint16_t)(rand() & 0xFF);
	}

	assert(false);
	return 0;
}

uint8_t C64Memory::peekAuto(uint16_t addr)
{		
	if (addr < 0xA000) {
		if (addr <= 0x0001) {
			// Processor port
			uint8_t dir = cpu->getPortDirection();
			return (addr == 0x0000) ? dir : (dir & cpu->getPort()) | (~dir & 0x17);
		} else {
			// RAM
			return ram[addr];
		}
	} else if (addr < 0xD000) {
		if (addr < 0xC000) {
			// Basic ROM
			return basicRomIsVisible ? rom[addr] : ram[addr];
		} else {
			// RAM
			return ram[addr];
		}
	} else {
		if (addr < 0xE000) {
			// Character ROM
			if (IOIsVisible) {			
				return peekIO(addr);
			} else {		
				return charRomIsVisible ? rom[addr] : ram[addr];
			}
		} else {
			// Kernel ROM
			return kernelRomIsVisible ? rom[addr] : ram[addr];
		}
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
	rom[addr] = value; 
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
}

void C64Memory::pokeIO(uint16_t addr, uint8_t value)
{
	// 0xD000 - 0xD3FF (VIC)
	if (VIC::isVicAddr(addr)) {
		// Note: Only the lower 6 bits are used for adressing the VIC I/O space
		// Therefore, the VIC I/O memory repeats every 64 bytes
		vic->poke(addr & 0x003F, value);	
		return;
	}
	
	// 0xD400 - 0xD7FF (SID)
	if (SID::isSidAddr(addr)) {
		// Note: Only the lower 5 bits are used for adressing the SID I/O space
		// Therefore, the SID I/O memory repeats every 32 bytes
		sid->poke(addr & 0x001F, value);
		return;
	}

	// 0xD800 - 0xDBFF (Color RAM)
	if (addr >= 0xD800 && addr <= 0xDBFF) {
		// Note: The color RAM only saves 4 Bit per address (one nibble)
		// When reading the color RAM, the upper 4 bits will contain random values
		colorRam[addr - 0xD800] = (value & 0x0F) | (rand() & 0xF0);
		return;
	}	

	// 0xDC00 - 0xDCFF (CIA 1)
	if (CIA1::isCia1Addr(addr)) {
		// Note: Only the lower 4 bits are used for adressing the CIA I/O space
		// Therefore, the VIC I/O memory repeats every 16 bytes
		cia1->poke(addr & 0x000F, value);
		return;
	}
	
	// 0xDD00 - 0xDDFF (CIA 2)	
	if (CIA2::isCia2Addr(addr)) {
		// Note: Only the lower 4 bits are used for adressing the CIA I/O space
		// Therefore, the VIC I/O memory repeats every 16 bytes
		cia2->poke(addr & 0x000F, value);
		return;
	}	
}


void C64Memory::pokeAuto(uint16_t addr, uint8_t value)
{	
	if (isCharRomAddr(addr)) {
		if (IOIsVisible) {		
			pokeIO(addr, value);
			return;
		}
	} else if (addr <= 0x0001) {
		if (addr == 0x0000) {
			cpu->setPortDirection(value);
			processorPortHasChanged(cpu->getPortLines());
		} else {
			cpu->setPort(value);
			processorPortHasChanged(cpu->getPortLines());
		}
		return;
	}
				
	// Default: Write to RAM
	ram[addr] = value;
}



