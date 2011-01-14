/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

Memory::Memory()
{	
	name = "MEM";
	
	cpu = NULL;
}

Memory::~Memory()
{
}

void
Memory::loadFromBuffer(uint8_t **buffer)
{	
}

void
Memory::saveToBuffer(uint8_t **buffer) 
{
}


// --------------------------------------------------------------------------------
//                                     Peek
// --------------------------------------------------------------------------------

uint8_t Memory::peekFrom(uint16_t addr, MemoryType source)
{
	switch (source) {
		case MEM_RAM: 
			return peekRam(addr);
		case MEM_ROM: 
			return peekRom(addr);
		case MEM_IO: 
			return peekIO(addr);
		default: 
			assert(false);
			return 0;
	}
}


// --------------------------------------------------------------------------------
//                                     Poke
// --------------------------------------------------------------------------------

void Memory::pokeTo(uint16_t addr, uint8_t value, MemoryType dest)
{
	switch (dest) {
		case MEM_RAM: 
			pokeRam(addr, value);
			break;
		case MEM_ROM: 
			pokeRom(addr, value);
			break;
		case MEM_IO:
			pokeIO(addr, value);
			break;
		default: 
			assert(false);
	}
}

void Memory::flashRom(const char *filename, uint16_t start)
{
	FILE *file;
	uint16_t addr = start;
	int  c;
		
	assert(start > 0);
	
	// Open and read
	if (!(file = fopen(filename, "r")))
		return;
	c = fgetc(file);
	while(c != EOF) {
		pokeRom(addr, (uint8_t)c); //  rom[addr] = c;
		c = fgetc(file);
		addr++;
	}
	fclose(file);
	
	debug(1, "ROM image %s loaded [%X;%X]\n", filename, (uint16_t)start, (uint16_t)(addr-1));
}
