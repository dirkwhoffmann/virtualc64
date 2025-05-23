// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "VirtualC64Config.h"
#include "Memory.h"
#include "Emulator.h"
#include "IOUtils.h"

#include <random>

namespace vc64 {

Memory::Memory(C64 &ref) : SubComponent(ref)
{    		
    memset(rom, 0, sizeof(rom));
    
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
    MemType map[32][6] = {
        
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::RAM,  MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::RAM,  MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::CRTHI, MemType::RAM,  MemType::CHAR, MemType::KERNAL },
        { MemType::RAM,  MemType::CRTLO, MemType::CRTHI, MemType::RAM,  MemType::CHAR, MemType::KERNAL },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::RAM,  MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::IO,   MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::CRTHI, MemType::RAM,  MemType::IO,   MemType::KERNAL },
        { MemType::RAM,  MemType::CRTLO, MemType::CRTHI, MemType::RAM,  MemType::IO,   MemType::KERNAL },
        
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::RAM,  MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::CHAR, MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::CHAR, MemType::KERNAL },
        { MemType::RAM,  MemType::CRTLO, MemType::BASIC, MemType::RAM,  MemType::CHAR, MemType::KERNAL },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::RAM,  MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::IO,   MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::IO,   MemType::KERNAL },
        { MemType::RAM,  MemType::CRTLO, MemType::BASIC, MemType::RAM,  MemType::IO,   MemType::KERNAL },
        
        { MemType::NONE, MemType::CRTLO, MemType::NONE,  MemType::NONE, MemType::IO,   MemType::CRTHI  },
        { MemType::NONE, MemType::CRTLO, MemType::NONE,  MemType::NONE, MemType::IO,   MemType::CRTHI  },
        { MemType::NONE, MemType::CRTLO, MemType::NONE,  MemType::NONE, MemType::IO,   MemType::CRTHI  },
        { MemType::NONE, MemType::CRTLO, MemType::NONE,  MemType::NONE, MemType::IO,   MemType::CRTHI  },
        { MemType::NONE, MemType::CRTLO, MemType::NONE,  MemType::NONE, MemType::IO,   MemType::CRTHI  },
        { MemType::NONE, MemType::CRTLO, MemType::NONE,  MemType::NONE, MemType::IO,   MemType::CRTHI  },
        { MemType::NONE, MemType::CRTLO, MemType::NONE,  MemType::NONE, MemType::IO,   MemType::CRTHI  },
        { MemType::NONE, MemType::CRTLO, MemType::NONE,  MemType::NONE, MemType::IO,   MemType::CRTHI  },
        
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::RAM,  MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::CHAR, MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::CHAR, MemType::KERNAL },
        { MemType::RAM,  MemType::RAM,   MemType::BASIC, MemType::RAM,  MemType::CHAR, MemType::KERNAL },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::RAM,  MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::IO,   MemType::RAM    },
        { MemType::RAM,  MemType::RAM,   MemType::RAM,   MemType::RAM,  MemType::IO,   MemType::KERNAL },
        { MemType::RAM,  MemType::RAM,   MemType::BASIC, MemType::RAM,  MemType::IO,   MemType::KERNAL }
    };
    
    for (isize i = 0; i < 32; i++) {
        
        bankMap[i][0x0] = MemType::PP;
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
    peekSrc[0x0] = pokeTarget[0x0] = MemType::PP;
    for (isize i = 0x1; i <= 0xF; i++) {
        peekSrc[i] = pokeTarget[i] = MemType::RAM;
    }
}

void
Memory::_didReset(bool hard)
{
    if (hard) {

        // Erase RAM
        eraseWithPattern(config.ramPattern);
        
        // Initialize color RAM with random numbers
        u32 seed = 0;
        for (isize i = 0; i < isizeof(colorRam); i++) {
            
            seed = c64.random(seed);
            colorRam[i] = u8(seed);
        }
    }
}

void 
Memory::operator << (SerCounter &worker)
{
    serialize(worker);
    if (config.saveRoms) worker << rom;
}

void 
Memory::operator << (SerReader &worker)
{
    serialize(worker);
    if (config.saveRoms) worker << rom;
}

void 
Memory::operator << (SerWriter &worker)
{
    serialize(worker);
    if (config.saveRoms) worker << rom;
}

void
Memory::eraseWithPattern(RamPattern pattern)
{
    /* Note: The RAM init pattern is not unique across C64 models (for details,
     * see the README file in the VICE test suite C64/raminitpattern). By
     * default, VirtualC64 utilizes the same patters as VICE. This pattern has
     * been selected, because it is pretty close to what can be seen on most
     * real machines and it make all four tests from the VICE test suite pass
     * (cyberloadtest.prg, darkstarbbstest.prg, platoontest.prg, and
     * typicaltet.prg). The CCS scheme is the one that is used by CCS 3.9. Note
     * that the darkstarbbstest fails with this pattern. The remainung patterns
     * can't be found in the wild. They allow the user to initialize the RAM
     * with all zeroes, all ones, or random values, respectively.
     */
    
    switch (pattern) {
            
        case RamPattern::VICE:
            
            // $00 $00 $FF $FF $FF $FF $00 $00 ...
            for (isize i = 0; i < isizeof(ram); i++)
                ram[i] = (i & 0x6) == 0x2 || (i & 0x6) == 0x4 ? 0xFF : 0x00;
            
            // In addition, the 2nd and 3rd 16K bank are inverted
            for (isize i = 0; i < isizeof(ram); i++)
                ram[i] ^= (i & 0x4000) ? 0xFF : 0x00;
            
            break;
            
        case RamPattern::CCS:
            
            // (64 x $FF) (64 x $00) ...
            for (isize i = 0; i < isizeof(ram); i++)
                ram[i] = (i & 0x40) ? 0x00 : 0xFF;
            
            break;
            
        case RamPattern::ZEROES:
            
            for (isize i = 0; i < isizeof(ram); i++)
                ram[i] = 0;
            
            break;
            
        case RamPattern::ONES:
            
            for (isize i = 0; i < isizeof(ram); i++)
                ram[i] = 0xFF;
            
            break;
            
        case RamPattern::RANDOM:
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, 0xFF);
            
            for (isize i = 0; i < isizeof(ram); i++)
                ram[i] = (u8)distrib(gen);
            
            break;
        }
        default:
            fatalError;
    }
}

void 
Memory::updatePeekPokeLookupTables()
{
    // Read game line, exrom line, and processor port bits
    u8 game  = expansionPort.getGameLine() ? 0x08 : 0x00;
    u8 exrom = expansionPort.getExromLine() ? 0x10 : 0x00;
    u8 index = (cpu.readPort() & 0x07) | exrom | game;
    
    // Set ultimax flag
    c64.setUltimax(exrom && !game);
    
    // Update table entries
    for (isize bank = 1; bank < 16; bank++) {
        peekSrc[bank] = pokeTarget[bank] = bankMap[index][bank];
    }
    
    // Call the Cartridge's delegation method
    expansionPort.updatePeekPokeLookupTables();
}

u8
Memory::peek(u16 addr, MemType source)
{
    if (config.heatmap) stats.reads[addr]++;

    switch(source) {
            
        case MemType::RAM:
            
            return ram[addr];
            
        case MemType::BASIC:
        case MemType::CHAR:
        case MemType::KERNAL:
            
            return rom[addr];
            
        case MemType::IO:
            
            return peekIO(addr);
            
        case MemType::CRTLO:
        case MemType::CRTHI:
            
            return expansionPort.peek(addr);
            
        case MemType::PP:
            
            if (likely(addr >= 0x02)) {
                return ram[addr];
            } else {
                return addr ? cpu.readPort() : cpu.readPortDir();
            }
            
        case MemType::NONE:
            
            return vic.getDataBusPhi1();
            
        default:
            fatalError;
    }
}

u8
Memory::peek(u16 addr, bool gameLine, bool exromLine)
{
    u8 game  = gameLine ? 0x08 : 0x00;
    u8 exrom = exromLine ? 0x10 : 0x00;
    u8 index = (cpu.readPort() & 0x07) | exrom | game;
    
    return peek(addr, bankMap[index][addr >> 12]);
}

u8
Memory::peekZP(u8 addr)
{
    if (config.heatmap) stats.reads[addr]++;

    if (likely(addr >= 0x02)) {
        return ram[addr];
    } else {
        return addr ? cpu.readPort() : cpu.readPortDir();
    }
}

u8
Memory::peekStack(u8 sp)
{
    if (config.heatmap) stats.reads[sp]++;

    return ram[0x100 + sp];
}

u8
Memory::peekIO(u16 addr)
{
    assert(addr >= 0xD000 && addr <= 0xDFFF);
    
    if (config.heatmap) stats.reads[addr]++;

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
            
            return sidBridge.peek(addr);
            
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
            
            return expansionPort.peekIO1(addr);
            
        case 0xF: // I/O space 2
            
            return expansionPort.peekIO2(addr);
    }
    
    fatalError;
}

u8
Memory::spypeek(u16 addr, MemType source) const
{
    switch(source) {
            
        case MemType::RAM:
            
            return ram[addr];
            
        case MemType::BASIC:
        case MemType::CHAR:
        case MemType::KERNAL:
            
            return rom[addr];
            
        case MemType::IO:
            
            return spypeekIO(addr);
            
        case MemType::CRTLO:
        case MemType::CRTHI:
            
            return expansionPort.spypeek(addr);
            
        case MemType::PP:
            
            if (addr >= 0x02) {
                return ram[addr];
            } else {
                return addr ? cpu.readPort() : cpu.readPortDir();
            }
            
        case MemType::NONE:
            
            return ram[addr];
            
        default:
            fatalError;
    }
}

u8
Memory::spypeekIO(u16 addr) const
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
            
            return sidBridge.spypeek(addr & 0x001F);
            
        case 0x8: // Color Ram
        case 0x9: // Color Ram
        case 0xA: // Color Ram
        case 0xB: // Color Ram
            return spypeekColor(addr - 0xD800);
            
        case 0xC: // CIA 1
            
            // Only the lower 4 bits are used for adressing the CIA I/O space.
            // As a result, CIA's I/O memory repeats every 16 bytes.
            return cia1.spypeek(addr & 0x000F);
            
        case 0xD: // CIA 2
            
            return cia2.spypeek(addr & 0x000F);
            
        case 0xE: // I/O space 1
            
            return expansionPort.spypeekIO1(addr);
            
        case 0xF: // I/O space 2
            
            return expansionPort.spypeekIO2(addr);
            
        default:
            fatalError;
    }
}

u8
Memory::spypeekColor(u16 addr) const
{
    assert(addr <= 0x400);
    return colorRam[addr];
}

void
Memory::poke(u16 addr, u8 value, MemType target)
{
    if (config.heatmap) stats.writes[addr]++;

    switch(target) {
            
        case MemType::RAM:
        case MemType::BASIC:
        case MemType::CHAR:
        case MemType::KERNAL:
            
            ram[addr] = value;
            return;
            
        case MemType::IO:
            
            pokeIO(addr, value);
            return;
            
        case MemType::CRTLO:
        case MemType::CRTHI:
            
            expansionPort.poke(addr, value);
            return;
            
        case MemType::PP:
            
            if (likely(addr >= 0x02)) {
                ram[addr] = value;
            } else {
                addr ? cpu.writePort(value) : cpu.writePortDir(value);
            }
            return;
            
        case MemType::NONE:
            
            return;
            
        default:
            fatalError;
    }
}

void
Memory::poke(u16 addr, u8 value, bool gameLine, bool exromLine)
{
    u8 game  = gameLine ? 0x08 : 0x00;
    u8 exrom = exromLine ? 0x10 : 0x00;
    u8 index = (cpu.readPort() & 0x07) | exrom | game;
    
    poke(addr, value, bankMap[index][addr >> 12]);
}

void
Memory::pokeZP(u8 addr, u8 value)
{
    if (config.heatmap) stats.writes[addr]++;

    if (likely(addr >= 0x02)) {
        ram[addr] = value;
    } else if (addr == 0x00) {
        cpu.writePortDir(value);
    } else {
        cpu.writePort(value);
    }
}

void
Memory::pokeStack(u8 sp, u8 value)
{
    if (config.heatmap) stats.writes[sp]++;

    ram[0x100 + sp] = value;
}

void
Memory::pokeIO(u16 addr, u8 value)
{
    assert(addr >= 0xD000 && addr <= 0xDFFF);
 
    if (config.heatmap) stats.writes[addr]++;

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

            sidBridge.poke(addr, value);
            return;

        case 0x7: // SID

            sidBridge.poke(addr, value);
            regressionTester.pokeDebugCart(addr, value);
            return;
            
        case 0x8: // Color RAM
        case 0x9: // Color RAM
        case 0xA: // Color RAM
        case 0xB: // Color RAM
            
            colorRam[addr - 0xD800] = (value & 0x0F) | (c64.random() & 0xF0);
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
            
            expansionPort.pokeIO1(addr, value);
            return;
            
        case 0xF: // I/O space 2
            
            expansionPort.pokeIO2(addr, value);
            return;
    }
    
    fatalError;
}

u16
Memory::nmiVector() const {
    
    if (peekSrc[0xF] != MemType::KERNAL || c64.hasRom(RomType::KERNAL)) {
        return LO_HI(spypeek(0xFFFA), spypeek(0xFFFB));
    } else {
        return 0xFE43;
    }
}

u16
Memory::irqVector() const {
    
    if (peekSrc[0xF] != MemType::KERNAL || c64.hasRom(RomType::KERNAL)) {
        return LO_HI(spypeek(0xFFFE), spypeek(0xFFFF));
    } else {
        return 0xFF48;
    }
}

u16
Memory::resetVector() {

    updatePeekPokeLookupTables();
    
    if (peekSrc[0xF] != MemType::KERNAL || c64.hasRom(RomType::KERNAL)) {
        return LO_HI(spypeek(0xFFFC), spypeek(0xFFFD));
    } else {
        return 0xFCE2;
    }
}

string
Memory::memdump(u16 addr, isize num, bool hex, isize pads, MemType src) const
{
    char result[128];
    char *p = result;
    
    assert(num <= 16);
    
    if (src == MemType::NONE) {
        
        for (isize i = 0; i < num; i++) {

            for (isize j = 0; j < pads; j++) *p++ = ' ';
            *p++ = '-'; *p++ = '-';
        }
        *p = 0;
        
    } else {
        
        for (isize i = 0; i < num; i++) {

            for (isize j = 0; j < pads; j++) *p++ = ' ';

            if (hex) {
                util::sprint8x(p, spypeek(addr++, src));
                p += 2;
            } else {
                util::sprint8d(p, spypeek(addr++, src));
                p += 3;
            }
        }
    }
    
    return string(result);
}

string
Memory::hexdump(u16 addr, isize num, isize pads, MemType src) const
{
    return memdump(addr, num, true, pads, src);
}

string
Memory::decdump(u16 addr, isize num, isize pads, MemType src) const
{
    return memdump(addr, num, false, pads, src);
}

string
Memory::txtdump(u16 addr, isize num, MemType src) const
{
    char result[17];
    char *p = result;
    
    assert(num <= 16);
    
    if (src != MemType::NONE) {
        
        for (isize i = 0; i < num; i++) {
            
            u8 byte = spypeek(addr++, src);
            
            if (byte >= 65 && byte <= 90) { *p++ = byte; continue; }
            if (byte >= 32 && byte <= 57) { *p++ = byte; continue; }
            *p++ = '.';
        }
    }
    *p = 0;
    
    return string(result);
}

void
Memory::memDump(std::ostream& os, u16 addr, isize numLines, bool hex)
{
    addr &= ~0xF;

    os << std::setfill('0') << std::hex << std::uppercase << std::right;

    for (isize i = 0, count = 16; i < numLines; i++) {

        os << std::setw(4) << isize(addr);
        os << ": ";
        os << memdump(addr, count, hex);
        os << "  ";
        os << txtdump(addr, count);
        os << std::endl;

        U16_INC(addr, count);
    }
}

void 
Memory::endFrame()
{
    if (config.heatmap) {
        heatmap.update(*this);
    }
}


}
