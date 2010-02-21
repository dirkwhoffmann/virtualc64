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
	cartridge = NULL;
	
	basicRomIsVisible = false;
	kernelRomIsVisible = false;
	charRomIsVisible = false;
	IOIsVisible = false;
	cartridgeRomIsVisible = false;
}

C64Memory::~C64Memory()
{
	debug("  Releasing main memory at address %p...\n", this);
}

void C64Memory::reset()
{
	debug ("  Resetting main memory...\n");
	
	// Zero out RAM...
	for (unsigned i = 0; i < sizeof(ram); i++)
		ram[i] = 0;
	
	// Initialize color memory...
	// It's important here to write in random values as some games peek the color RAM 
	// to generate random numbers. E.g., Paradroid is doing it that way.
	for (unsigned i = 0; i < sizeof(colorRam); i++)
		colorRam[i] = rand();
	
	// Initialize processor port data direction register and processor port
	poke(0x0000, 0x2F); // Data direction
	poke(0x0001, 0x1F); // IO port, set default memory layout
}

// --------------------------------------------------------------------------------
//                                      Input / Output
// --------------------------------------------------------------------------------

bool
C64Memory::load(uint8_t **buffer)
{	
	debug("  Loading C64 memory state...\n");
	
	Memory::load(buffer);
	
	for (unsigned i = 0; i < sizeof(ram); i++)
		ram[i] = read8(buffer);	
	for (unsigned i = 0; i < sizeof(colorRam); i++) 
		colorRam[i] = read8(buffer);
	
	basicRomIsVisible = (bool)read8(buffer);
	charRomIsVisible = (bool)read8(buffer);
	kernelRomIsVisible = (bool)read8(buffer);
	IOIsVisible = (bool)read8(buffer);
	cartridgeRomIsVisible = (bool)read8(buffer);
	
	return true;
}

bool
C64Memory::save(uint8_t **buffer) 
{
	debug("  Saving C64 memory state...\n");
	
	Memory::save(buffer);

	for (unsigned i = 0; i < sizeof(ram); i++)
		write8(buffer, ram[i]);
	for (unsigned i = 0; i < sizeof(colorRam); i++) 
		write8(buffer, colorRam[i]);
	
	write8(buffer, (uint8_t)basicRomIsVisible);
	write8(buffer, (uint8_t)charRomIsVisible);
	write8(buffer, (uint8_t)kernelRomIsVisible);
	write8(buffer, (uint8_t)IOIsVisible);
	write8(buffer, (uint8_t)cartridgeRomIsVisible);

	return true;
}

void 
C64Memory::dumpState()
{
	debug("C64 Memory:\n");
	debug("-----------\n");
	debug("    Basic ROM :%s loaded,%s visible\n", basicRomIsLoaded() ? "" : " not", basicRomIsVisible ? "" : " not");
	debug("Character ROM :%s loaded,%s visible\n", charRomIsLoaded() ? "" : " not", charRomIsVisible ? "" : " not");
	debug("   Kernel ROM :%s loaded,%s visible\n", kernelRomIsLoaded() ? "" : " not", kernelRomIsVisible ? "" : " not");

	for (uint16_t i = 0; i < 0xFFFF; i++) {
		uint8_t tag = cpu->getBreakpointTag(i);
		if (tag != CPU::NO_BREAKPOINT) {
			debug("Breakpoint at %0x4X %s\n", i, tag == CPU::SOFT_BREAKPOINT ? "(soft)" : "");
		}
	}
	debug("\n");
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
		return (colorRam[addr - 0xD800] & 0x0F) | (rand() << 4);
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
		return (uint8_t)(rand());
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
			uint8_t ext = cpu->getExternalPortBits();
			return (addr == 0x0000) ? dir : (dir & cpu->getPort()) | (~dir & ext); // (~dir & 0x5F); // (~dir & 0x7F); //   (~dir & 0x17);
		} else if (addr < 0x8000){
			return ram[addr];
		} else {
			// RAM, or Cartridge ROM from $8000 - $9FFF
			return cartridgeRomIsVisible ? rom[addr] : ram[addr];
		}
	} else if (addr < 0xD000) {
		if (addr < 0xC000) {
			// Basic ROM, or Cartridge ROM from $A000 - $BFFF
			// Is it ok for the cartridge bank to overwrite the basic ROM data?
			return basicRomIsVisible || cartridgeRomIsVisible ? rom[addr] : ram[addr];
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
	
	// 0xDE00 - 0xDEFF (I/O area 1)
	// 0xDF00 - 0xDFFF (I/O area 2) 
	if (addr >= 0xDE00 && addr <= 0xDFFF) {
		// Expansion port I/O.
		if (cartridgeRomIsVisible) {

			if (addr == 0xDE00) {
				// For some cartridges (e.g. Ocean .crt type 5):
				// Bank switching is done by writing to $DE00. The lower six bits give the
				// bank number (ranging from 0-63). Bit 8 in this selection word is always
				// set.
				// When this occurs, the cartridge will present the selected bank
				// at the specified ROM locations.
				unsigned int bankNumber = 63 & value;
			
				printf("Switching to bank %d\n", bankNumber);
				
				// Bank cartridge chip into rom:
				// Because the cartridge address ranges $8000 - $9FFF and $A000 - $BFFF
				// are not used by kernal, basic, or other ROM, it is safe to copy 
				// the bank from the cartridge chip directly into the rom array.
				
				Cartridge::Chip *chip = cartridge->getChip(bankNumber);
				memcpy(&rom[chip->loadAddress], chip->rom, chip->size);
				
				printf("Banked %d bytes to 0x%04x\n", chip->size, chip->loadAddress);
			}
		}
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

bool C64Memory::attachCartridge(Cartridge *c)
{
	cartridge = c;
	
	// Bank first chip into rom
	
	Cartridge::Chip *chip = cartridge->getChip(0);
	assert(chip);
	memcpy(&rom[chip->loadAddress], chip->rom, chip->size);
	
	printf("Banked %d bytes to 0x%04x\n", chip->size, chip->loadAddress);
	
	// Cartridge rom is visible when EXROM or GAME lines are pulled low.
	cartridgeRomIsVisible = c->exromIsHigh()==false || c->gameIsHigh()==false;
	
	return true;
}

bool C64Memory::detachCartridge()
{
	delete cartridge;
	cartridge = NULL;
	
	cartridgeRomIsVisible = false;
	
	return true;
}


