// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

#define CHECK_WATCHPOINT(x) \
if (checkWatchpoints && cpu.debugger.watchpointMatches(x)) { \
    c64.signalWatchpoint(); \
}

C64Memory::C64Memory(C64 &ref) : C64Component(ref)
{    		
    memset(rom, 0, sizeof(rom));

    config.ramPattern = RAM_PATTERN_C64;
    config.debugcart = false;

    /* Memory bank map
     *
     * If x == (EXROM, GAME, CHAREN, HIRAM, LORAM) then
     *
     *   map[x][0] == mapping for range $1000 - $7FFF
     *   map[x][1] == mapping for range $8000 - $9FFF
     *   map[x][2] == mapping for range $A000 - $BFFF
     *   map[x][3] == mapping for range $C000 - $CFFF
     *   map[x][4] == mapping for range $D000 - $DFFF
     *   map[x][5] == mapping for range $E000 - $FFFF
     */
    MemoryType map[32][6] = {
        
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM    },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM    },
        { M_RAM,  M_RAM,   M_CRTHI, M_RAM,  M_CHAR, M_KERNAL },
        { M_RAM,  M_CRTLO, M_CRTHI, M_RAM,  M_CHAR, M_KERNAL },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM    },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM    },
        { M_RAM,  M_RAM,   M_CRTHI, M_RAM,  M_IO,   M_KERNAL },
        { M_RAM,  M_CRTLO, M_CRTHI, M_RAM,  M_IO,   M_KERNAL },
        
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM    },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_RAM    },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_KERNAL },
        { M_RAM,  M_CRTLO, M_BASIC, M_RAM,  M_CHAR, M_KERNAL },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM    },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM    },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_KERNAL },
        { M_RAM,  M_CRTLO, M_BASIC, M_RAM,  M_IO,   M_KERNAL },
        
        { M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI  },
        { M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI  },
        { M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI  },
        { M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI  },
        { M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI  },
        { M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI  },
        { M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI  },
        { M_NONE, M_CRTLO, M_NONE,  M_NONE, M_IO,   M_CRTHI  },
        
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM    },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_RAM    },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_CHAR, M_KERNAL },
        { M_RAM,  M_RAM,   M_BASIC, M_RAM,  M_CHAR, M_KERNAL },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_RAM,  M_RAM    },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_RAM    },
        { M_RAM,  M_RAM,   M_RAM,   M_RAM,  M_IO,   M_KERNAL },
        { M_RAM,  M_RAM,   M_BASIC, M_RAM,  M_IO,   M_KERNAL }
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
    
    // Initialize peekSource and pokeTarket tables
    peekSrc[0x0] = pokeTarget[0x0] = M_PP;
    for (unsigned i = 0x1; i <= 0xF; i++) {
        peekSrc[i] = pokeTarget[i] = M_RAM;
    }
}

void
C64Memory::_reset()
{
    RESET_SNAPSHOT_ITEMS
    
    // Erase RAM
    eraseWithPattern(config.ramPattern);
        
    // Initialize color RAM with random numbers
    srand(1000);
    for (unsigned i = 0; i < sizeof(colorRam); i++) {
        colorRam[i] = (rand() & 0xFF);
    }
}

long
C64Memory::getConfigItem(Option option)
{
    switch (option) {
            
        case OPT_RAM_PATTERN:  return config.ramPattern;
        case OPT_DEBUGCART:    return config.debugcart;
            
        default:
            assert(false);
            return 0;
    }
}

bool
C64Memory::setConfigItem(Option option, long value)
{
    switch (option) {
            
        case OPT_RAM_PATTERN:
            
            if (!RamPatternEnum::verify(value)) return false;
            if (config.ramPattern == value) return false;
            
            config.ramPattern = (RamPattern)value;
            return true;
            
        case OPT_DEBUGCART:
            
            if (config.debugcart == value) {
                return false;
            }
            
            config.debugcart = value;
            if (value) msg("Debug cart enabled\n");
            return true;
            
        default:
            return false;
    }
}

void
C64Memory::_inspect()
{
    synchronized {
        
        info.exrom = expansionport.getExromLine();
        info.game = expansionport.getGameLine();
        info.loram = cpu.pport.getLoram();
        info.hiram = cpu.pport.getHiram();
        info.charen = cpu.pport.getCharen();
        
        info.bankMap = cpu.pport.read();
        if (expansionport.getGameLine()) info.bankMap |= 0x08;
        if (expansionport.getExromLine()) info.bankMap |= 0x10;
        
        for (int i = 0; i < 16; i++) info.peekSrc[i] = peekSrc[i];
        for (int i = 0; i < 16; i++) info.vicPeekSrc[i] = vic.memSrc[i];
    }
}

void 
C64Memory::_dump()
{
	msg("C64 Memory:\n");
	msg("-----------\n");
    msg("    Basic ROM: %s loaded\n", c64.hasRom(ROM_TYPE_BASIC)  ? "" : " not");
	msg("Character ROM: %s loaded\n", c64.hasRom(ROM_TYPE_CHAR)   ? "" : " not");
    msg("   Kernal ROM: %s loaded\n", c64.hasRom(ROM_TYPE_KERNAL) ? "" : " not");
	msg("\n");
}

void
C64Memory::eraseWithPattern(RamPattern pattern)
{
    if (!RamPatternEnum::isValid(pattern)) {
        warn("Unknown RAM init pattern. Falling back to default.\n");
        pattern = RAM_PATTERN_C64;
    }
    
    if (pattern == RAM_PATTERN_C64) {
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
    // Read game line, exrom line, and processor port bits
    u8 game  = expansionport.getGameLine() ? 0x08 : 0x00;
    u8 exrom = expansionport.getExromLine() ? 0x10 : 0x00;
    u8 index = (cpu.pport.read() & 0x07) | exrom | game;

    // Set ultimax flag
    c64.setUltimax(exrom && !game);

    // Update table entries
    for (unsigned bank = 1; bank < 16; bank++) {
        peekSrc[bank] = pokeTarget[bank] = bankMap[index][bank];
    }
    
    // Call the Cartridge's delegation method
    expansionport.updatePeekPokeLookupTables();
}

u8
C64Memory::peek(u16 addr, MemoryType source)
{
    CHECK_WATCHPOINT(addr)
    
    switch(source) {
        
        case M_RAM:
        return ram[addr];
        
        case M_BASIC:
        case M_CHAR:
        case M_KERNAL:
        return rom[addr];
        
        case M_IO:
        return peekIO(addr);
        
        case M_CRTLO:
        case M_CRTHI:
        return expansionport.peek(addr);
        
        case M_PP:
        if (likely(addr >= 0x02)) {
            return ram[addr];
        } else if (addr == 0x00) {
            return cpu.pport.readDirection();
        } else {
            return cpu.pport.read();
        }
        
        case M_NONE:
        return vic.getDataBusPhi1();
        
        default:
        assert(false);
        return 0;
    }
}

u8
C64Memory::peek(u16 addr, bool gameLine, bool exromLine)
{
    u8 game  = gameLine ? 0x08 : 0x00;
    u8 exrom = exromLine ? 0x10 : 0x00;
    u8 index = (cpu.pport.read() & 0x07) | exrom | game;
    
    return peek(addr, bankMap[index][addr >> 12]);
}

u8
C64Memory::peekZP(u8 addr)
{
    CHECK_WATCHPOINT(addr)
    
    if (likely(addr >= 0x02)) {
        return ram[addr];
    } else if (addr == 0x00) {
        return cpu.pport.readDirection();
    } else {
        return cpu.pport.read();
    }
}

u8
C64Memory::peekStack(u8 sp)
{
    CHECK_WATCHPOINT(sp)
    
    return ram[0x100 + sp];
}

u8
C64Memory::peekIO(u16 addr)
{
    CHECK_WATCHPOINT(addr)
    
    assert(addr >= 0xD000 && addr <= 0xDFFF);
    
    switch ((addr >> 8) & 0xF) {
            
        case 0x0: // VICII
        case 0x1: // VICII
        case 0x2: // VICII
        case 0x3: // VICII
            
            // Only the lower 6 bits are used for adressing the VIC I/O space.
            // As a result, VICII's I/O memory repeats every 64 bytes.
            return vic.peek(addr & 0x003F);

        case 0x4: // SID
        case 0x5: // SID
        case 0x6: // SID
        case 0x7: // SID
            
            return sid.peek(addr);

        case 0x8: // Color RAM
        case 0x9: // Color RAM
        case 0xA: // Color RAM
        case 0xB: // Color RAM
 
            return (colorRam[addr - 0xD800] & 0x0F) | (vic.getDataBusPhi1() & 0xF0);

        case 0xC: // CIA 1
 
            // Only the lower 4 bits are used for adressing the CIA I/O space.
            // As a result, CIA's I/O memory repeats every 16 bytes.
            return cia1.peek(addr & 0x000F);
	
        case 0xD: // CIA 2
            
            return cia2.peek(addr & 0x000F);
            
        case 0xE: // I/O space 1
            
            return expansionport.peekIO1(addr);
            
        case 0xF: // I/O space 2

            return expansionport.peekIO2(addr);
	}
    
	assert(false);
	return 0;
}

u8
C64Memory::spypeek(u16 addr, MemoryType source)
{
    switch(source) {
            
        case M_RAM:
            return ram[addr];
            
        case M_BASIC:
        case M_CHAR:
        case M_KERNAL:
            return rom[addr];
            
        case M_IO:
            return spypeekIO(addr);
            
        case M_CRTLO:
        case M_CRTHI:
            return expansionport.spypeek(addr);
            
        case M_PP:
            return peek(addr, M_PP);
      
        case M_NONE:
            return ram[addr];
            
        default:
            assert(false);
            return 0;
    }
}

u8
C64Memory::spypeekIO(u16 addr)
{
    assert(addr >= 0xD000 && addr <= 0xDFFF);
    
    switch ((addr >> 8) & 0xF) {
            
        case 0x0: // VICII
        case 0x1: // VICII
        case 0x2: // VICII
        case 0x3: // VICII
            
            return vic.spypeek(addr & 0x003F);
            
        case 0x4: // SID
        case 0x5: // SID
        case 0x6: // SID
        case 0x7: // SID
            
            return sid.spypeek(addr & 0x001F);
            
        case 0xC: // CIA 1
            
            // Only the lower 4 bits are used for adressing the CIA I/O space.
            // As a result, CIA's I/O memory repeats every 16 bytes.
            return cia1.spypeek(addr & 0x000F);
            
        case 0xD: // CIA 2
            
            return cia2.spypeek(addr & 0x000F);
            
        case 0xE: // I/O space 1
            
            return expansionport.spypeekIO1(addr);
            
        case 0xF: // I/O space 2
            
            return expansionport.spypeekIO2(addr);

        default:
            
            return peek(addr);
    }
}

u8
C64Memory::spypeekColor(u16 addr)
{
    assert(addr <= 0x400);
    return colorRam[addr];
}

void
C64Memory::poke(u16 addr, u8 value, MemoryType target)
{
    CHECK_WATCHPOINT(addr)
    
    switch(target) {
            
        case M_RAM:
        case M_BASIC:
        case M_CHAR:
        case M_KERNAL:
            ram[addr] = value;
            return;
            
        case M_IO:
            pokeIO(addr, value);
            return;
            
        case M_CRTLO:
        case M_CRTHI:
            expansionport.poke(addr, value);
            return;
            
        case M_PP:
            if (likely(addr >= 0x02)) {
                ram[addr] = value;
            } else if (addr == 0x00) {
                cpu.pport.writeDirection(value);
            } else {
                cpu.pport.write(value);
            }
            return;
            
        case M_NONE:
            return;
            
        default:
            assert(false);
            return;
    }
}

void
C64Memory::poke(u16 addr, u8 value, bool gameLine, bool exromLine)
{
    u8 game  = gameLine ? 0x08 : 0x00;
    u8 exrom = exromLine ? 0x10 : 0x00;
    u8 index = (cpu.pport.read() & 0x07) | exrom | game;
    
    poke(addr, value, bankMap[index][addr >> 12]);
}

void
C64Memory::pokeZP(u8 addr, u8 value)
{
    CHECK_WATCHPOINT(addr)
    
    if (likely(addr >= 0x02)) {
        ram[addr] = value;
    } else if (addr == 0x00) {
        cpu.pport.writeDirection(value);
    } else {
        cpu.pport.write(value);
    }
}

void
C64Memory::pokeStack(u8 sp, u8 value)
{
    CHECK_WATCHPOINT(sp)
    
    ram[0x100 + sp] = value;
}

void
C64Memory::pokeIO(u16 addr, u8 value)
{
    CHECK_WATCHPOINT(addr)
    
    assert(addr >= 0xD000 && addr <= 0xDFFF);
    
    switch ((addr >> 8) & 0xF) {
            
        case 0x0: // VICII
        case 0x1: // VICII
        case 0x2: // VICII
        case 0x3: // VICII
            
            // Only the lower 6 bits are used for adressing the VICII I/O space.
            // As a result, VICII's I/O memory repeats every 64 bytes.
            vic.poke(addr & 0x003F, value);
            return;
            
        case 0x4: // SID
        case 0x5: // SID
        case 0x6: // SID
        case 0x7: // SID
            
            sid.poke(addr, value);

            // Check the exit register (option -debugcart)
            if (addr == 0xD7FF && config.debugcart) {
                msg("DEBUGCART: Terminating with exit code %x\n", value);
                exit(value);
            }

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
            cia1.poke(addr & 0x000F, value);
            return;
            
        case 0xD: // CIA 2
            
            cia2.poke(addr & 0x000F, value);
            return;
            
        case 0xE: // I/O space 1
            
            expansionport.pokeIO1(addr, value);
            return;
            
        case 0xF: // I/O space 2
            
            expansionport.pokeIO2(addr, value);
            return;
    }
    
    assert(false);
}

u16
C64Memory::nmiVector() {
    
    if (peekSrc[0xF] != M_KERNAL || c64.hasRom(ROM_TYPE_KERNAL)) {
        return LO_HI(peek(0xFFFA), peek(0xFFFB));
    } else {
        return 0xFE43;
    }
}

u16
C64Memory::irqVector() {
    
    if (peekSrc[0xF] != M_KERNAL || c64.hasRom(ROM_TYPE_KERNAL)) {
        return LO_HI(peek(0xFFFE), peek(0xFFFF));
    } else {
        return 0xFF48;
    }
}

u16
C64Memory::resetVector() {
    
    if (peekSrc[0xF] != M_KERNAL || c64.hasRom(ROM_TYPE_KERNAL)) {
        return LO_HI(peek(0xFFFC), peek(0xFFFD));
    } else {
        return 0xFCE2;
    }
}

char *
C64Memory::memdump(u16 addr, long num, bool hex, MemoryType src)
{
    static char result[128];
    char *p = result;
    
    assert(num <= 16);

    if (src == M_NONE) {
        
        for (int i = 0; i < num; i++) {
            if (hex) {
                *p++ = ' '; *p++ = ' '; *p++ = '-'; *p++ = '-';
            } else {
                *p++ = ' '; *p++ = '-'; *p++ = '-'; *p++ = '-';
            }
        }
        *p = 0;

    } else {
        
        for (int i = 0; i < num; i++) {
            if (hex) {
                *p++ = ' ';
                *p++ = ' ';
                sprint8x(p, spypeek(addr++, src));
                p += 2;
            } else {
                *p++ = ' ';
                sprint8d(p, spypeek(addr++, src));
                p += 3;
            }
        }
    }
    
    return result;
}

char *
C64Memory::txtdump(u16 addr, long num, MemoryType src)
{
    static char result[17];
    char *p = result;
    
    assert(num <= 16);
    
    if (src != M_NONE) {
        
        for (int i = 0; i < num; i++) {
            
            u8 byte = spypeek(addr++, src);
            
            // if (byte >=  1 && byte <= 26) { *p++ = (byte - 1) + 'A'; continue; }
            // if (byte >= 48 && byte <= 57) { *p++ = (byte - 48) + '0'; continue; }
            if (byte >= 65 && byte <= 90) { *p++ = byte; continue; }
            if (byte >= 32 && byte <= 57) { *p++ = byte; continue; }
            *p++ = '.';
        }
    }
    *p = 0;

    return result;
}
