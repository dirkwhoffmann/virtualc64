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

VC1541Memory::VC1541Memory()
{
	debug("  Creating VC1541 memory at %p...\n", this);	

	iec = NULL;
	floppy = NULL;
	romFile = NULL;
}

VC1541Memory::~VC1541Memory()
{
	debug("  Releasing VC1541 memory at %p...\n", this);
}

void 
VC1541Memory::reset()
{
	debug ("    Resetting VC1541 memory...\n");

	// Zero out RAM...
	for (unsigned i = 0; i < 0xC000; i++)
		mem[i] = 0;	
}	

bool 
VC1541Memory::load(uint8_t **buffer)
{
	debug("    Loading VC1541 memory state...\n");

	for (unsigned i = 0; i < 0xC000; i++)
		mem[i] = read8(buffer);	
	return true;
}

bool 
VC1541Memory::save(uint8_t **buffer)
{
	debug("    Saving VC1541 memory state...\n");

	for (unsigned i = 0; i < 0xC000; i++)
		write8(buffer, mem[i]);
	return true;
}

bool 
VC1541Memory::is1541Rom(const char *filename)
{
	int magic_bytes[] = { 0x97, 0xAA, 0xAA, EOF };

	assert(filename != NULL);
	
	if (!checkFileSize(filename, 0x4000, 0x4000))
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
		
	return true;
}

bool 
VC1541Memory::loadRom(const char *filename)
{
	if (is1541Rom(filename)) {
		romFile = strdup(filename);
		flashRom(filename, 0xC000);
		return true;
	}
	return false;
}

void 
VC1541Memory::dumpState()
{
	debug("VC1541 Memory:\n");
	debug("--------------\n\n");
	debug("VC1541 ROM :%s loaded\n", romIsLoaded() ? "" : " not");
	for (uint16_t i = 0; i < 0xFFFF; i++) {
		uint8_t tag = cpu->getBreakpointTag(i);
		if (tag != CPU::NO_BREAKPOINT) {
			debug("Breakpoint at %0x4X %s\n", i, tag == CPU::SOFT_BREAKPOINT ? "(soft)" : "");
		}
	}
	for (uint16_t i = 0; i < 0xFFFF; i++) {
		uint8_t tag = getWatchpointType(i);
		if (tag == WATCH_FOR_VALUE) {
			debug("Watchpoint at %0x4X (watch for value %02X)\n", i, getWatchValue(i));
		} else if (tag == WATCH_FOR_ALL) {
			debug("Watchpoint at %0x4X\n", i);
		}
	}
	debug("\n");
}

bool 
VC1541Memory::isValidAddr(uint16_t addr, MemoryType type)
{
	switch (type) {
		case MEM_RAM: 
			return (addr < 0x800);
		case MEM_ROM:
			return (addr >= 0xC000);
		case MEM_IO:
			return (addr >= 0x1800 && addr <= 0x1c00) || (addr >= 0x1c00 && addr <= 0x2000);
		case MEM_WATCHPOINT:
			return getWatchpointType(addr) != NO_WATCHPOINT;
		default:
			assert(false);
			return false;
	}
}

uint8_t 
VC1541Memory::peekRam(uint16_t addr)
{
	return mem[addr];
}

uint8_t 
VC1541Memory::peekRom(uint16_t addr)
{
	return mem[addr];
}

uint8_t 
VC1541Memory::peekIO(uint16_t addr)
{	
	if ((addr & 0xFC00) == 0x1800) {
		return floppy->via1->peek(addr & 0x000F);
	} else if ((addr & 0xFC00) == 0x1c00) {
		return floppy->via2->peek(addr & 0x000F);
	} else {
		debug("PANIC: Wrong VC1541 IO memory address\n");
		assert(0);
		return 0;
	}
}

uint8_t 
VC1541Memory::peekAuto(uint16_t addr)
{
	if (addr >= 0xc000)
		return mem[addr];
	else if (addr < 0x1000)
		return mem[addr & 0x07ff];  // RAM repeats multiple times
	else
		return peekIO(addr);
}
	                  																
void 
VC1541Memory::pokeRam(uint16_t addr, uint8_t value)
{
	mem[addr] = value;
}

void 
VC1541Memory::pokeRom(uint16_t addr, uint8_t value)
{
	mem[addr] = value;
}
             
void 
VC1541Memory::pokeIO(uint16_t addr, uint8_t value)
{	
	if ((addr & 0xFC00) == 0x1800) {
		floppy->via1->poke(addr & 0x000F, value);
	} else if ((addr & 0xFC00) == 0x1c00) {
		floppy->via2->poke(addr & 0X000F, value);
	} else {
		debug("PANIC: Wrong VC1541 IO memory address\n");
		assert(0);
	}
}
			 			 
void 
VC1541Memory::pokeAuto(uint16_t addr, uint8_t value)
{
	if (addr < 0x1000)
		mem[addr & 0x7ff] = value;
	else
		pokeIO(addr, value);
}


