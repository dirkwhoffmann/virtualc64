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

VC1541Memory::VC1541Memory()
{
    setDescription("1541MEM");
	debug(3, "  Creating VC1541 memory at %p...\n", this);

    // Register snapshot items
    SnapshotItem items[] = {

    { mem,              0xC000,     CLEAR_ON_RESET },
    { &mem[0xC000],     0x4000,     KEEP_ON_RESET  }, /* VC1541 Rom */
    { NULL,             0,          0 }};

    registerSnapshotItems(items, sizeof(items));

	romFile = NULL;
}

VC1541Memory::~VC1541Memory()
{
	debug(3, "  Releasing VC1541 memory at %p...\n", this);
}

void 
VC1541Memory::reset()
{
    VirtualComponent::reset();
}

//
// Handling ROM images
//

bool 
VC1541Memory::is1541Rom(const char *filename)
{
	uint8_t magicBytes1[] = { 0x97, 0xAA, 0xAA, 0x00 };
	uint8_t magicBytes2[] = { 0x97, 0xE0, 0x43, 0x00 };

	if (filename == NULL)
		return false;
	
	if (!checkFileSize(filename, 0x4000, 0x4000))
		return false;
	
	if (!checkFileHeader(filename, magicBytes1) && !checkFileHeader(filename, magicBytes2))
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
	msg("VC1541 Memory:\n");
	msg("--------------\n\n");
	msg("VC1541 ROM :%s loaded\n", romIsLoaded() ? "" : " not");
	msg("\n");
}

uint8_t
VC1541Memory::peek(uint16_t addr, MemoryType source)
{
    // In contrast to the C64 where certain memorys overlap each other,
    // the type of each memory location in the VC1541 is unique.
    // Hence, we simply do some consistency checking here.
    
    if (addr >= 0x8000) { assert(source == M_ROM); }
    else if ((addr & 0x1FFF) < 0x0800) { assert(source == M_RAM); }
    else if ((addr & 0x1FFF) < 0x1800) { assert(source == M_NONE); }
    else { assert(source == M_IO); }
    
    return peek(addr);
}

uint8_t 
VC1541Memory::peek(uint16_t addr)
{
    if (addr >= 0x8000) {
        
        // 0xC000 - 0xFFFF : ROM
        // 0x8000 - 0xBFFF : ROM (repeated)
        return mem[addr | 0xC000];
        
    } else {
        
        // Map to range 0x0000 - 0x1FFF
        addr &= 0x1FFF;
        
        // 0x0000 - 0x07FF : RAM
        // 0x0800 - 0x17FF : unmapped
        // 0x1800 - 0x1BFF : VIA 1 (repeats every 16 bytes)
        // 0x1C00 - 0x1FFF : VIA 2 (repeats every 16 bytes)
        return
        (addr < 0x0800) ? mem[addr] :
        (addr < 0x1800) ? addr >> 8 :
        (addr < 0x1C00) ? floppy->via1.peek(addr & 0xF) :
        floppy->via2.peek(addr & 0xF);
    }
}

uint8_t
VC1541Memory::snoop(uint16_t addr, MemoryType source)
{
    // In contrast to the C64 where certain memorys overlap each other,
    // the type of each memory location in the VC1541 is unique.
    // Hence, we simply do some consistency checking here.
    
    if (addr >= 0x8000) { assert(source == M_ROM); }
    else if ((addr & 0x1FFF) < 0x0800) { assert(source == M_RAM); }
    else if ((addr & 0x1FFF) < 0x1800) { assert(source == M_NONE); }
    else { assert(source == M_IO); }
    
    return snoop(addr);
}

uint8_t
VC1541Memory::snoop(uint16_t addr)
{
    uint8_t result;
    
    if (addr >= 0xc000) {
        // ROM
        result = mem[addr];
    } else if (addr < 0x1000) {
        result = mem[addr & 0x07ff];
    } else {
        result = snoopIO(addr);
    }
    
    return result;
}

uint8_t
VC1541Memory::snoopIO(uint16_t addr)
{
    if ((addr & 0xFC00) == 0x1800) {
        return floppy->via1.snoop(addr & 0x000F);
    } else if ((addr & 0xFC00) == 0x1c00) {
        return floppy->via2.snoop(addr & 0x000F);
    } else {
        return (addr >> 8);
    }
}

void
VC1541Memory::poke(uint16_t addr, uint8_t value, MemoryType target)
{
    if (target == M_ROM) {
        assert(addr >= 0x8000);
        mem[addr] = value;
    } else {
        poke(addr, value);
    }
}

void 
VC1541Memory::poke(uint16_t addr, uint8_t value)
{
    if (addr >= 0x8000) { // ROM
        return;
    }
    
    // Map to range 0x0000 - 0x1FFF
    addr &= 0x1FFF;
    
    if (addr < 0x0800) { // RAM
        mem[addr] = value;
        return;
    }
    
    if (addr >= 0x1C00) { // VIA 2
        floppy->via2.poke(addr & 0xF, value);
        return;
    }
    
    if (addr >= 0x1800) { // VIA 1
        floppy->via1.poke(addr & 0xF, value);
        return;
    }
}

