//
//  Cartridge.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 20.01.18.
//

#include "C64.h"

Cartridge::Cartridge()
{
    setDescription("Cartridge");
    debug(3, "  Creating cartridge at address %p...\n", this);
    
    // We reset the cartridge here, as C64::reset() keeps the cartridge intact.
    reset();
}

Cartridge::~Cartridge()
{
    debug(3, "  Releasing cartridge port...\n");
    // detachCartridge();
}

void
Cartridge::reset()
{
    type = CRT_NONE;
    gameLine = true;
    exromLine = true;
    
    memset(rom, 0, sizeof(rom));
    memset(blendedIn, 0, sizeof(blendedIn));
    
    for (unsigned i = 0; i < 64; i++) {
        chip[i] = NULL;
        chipStartAddress[i] = 0;
        chipSize[i] = 0;
    }
}

void
Cartridge::ping()
{
}

uint32_t
Cartridge::stateSize()
{
    uint32_t size = 3;
    
    for (unsigned i = 0; i < 64; i++)
        size += 4 + chipSize[i];
    
    size += sizeof(rom);
    size += sizeof(blendedIn);
    
    return size;
}

void
Cartridge::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    type = read8(buffer);
    gameLine = (bool)read8(buffer);
    exromLine = (bool)read8(buffer);
    
    for (unsigned i = 0; i < 64; i++) {
        chipStartAddress[i] = read16(buffer);
        chipSize[i] = read16(buffer);
        
        if (chipSize[i] > 0) {
            chip[i] = (uint8_t *)malloc(chipSize[i]);
            readBlock(buffer, chip[i], chipSize[i]);
        } else {
            chip[i] = NULL;
        }
    }
    
    readBlock(buffer, rom, sizeof(rom));
    readBlock(buffer, blendedIn, sizeof(blendedIn));
    
    debug(2, "  Cartridge state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
Cartridge::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    write8(buffer, type);
    write8(buffer, (uint8_t)gameLine);
    write8(buffer, (uint8_t)exromLine);
    
    for (unsigned i = 0; i < 64; i++) {
        write16(buffer, chipStartAddress[i]);
        write16(buffer, chipSize[i]);
        
        if (chipSize[i] > 0) {
            writeBlock(buffer, chip[i], chipSize[i]);
        }
    }
    
    writeBlock(buffer, rom, sizeof(rom));
    writeBlock(buffer, blendedIn, sizeof(blendedIn));
    
    debug(4, "  Cartridge state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
Cartridge::dumpState()
{
    msg("Cartridge\n");
    msg("---------\n");
    
    msg("Cartridge type: %d\n", getCartridgeType());
    msg("Game line:      %d\n", getGameLine());
    msg("Exrom line:     %d\n", getExromLine());
    
    for (unsigned i = 0; i < 64; i++) {
        if (chip[i] != NULL) {
            msg("Chip %2d:        %d KB starting at $%04X\n", i, chipSize[i] / 1024, chipStartAddress[i]);
        }
    }
}

unsigned
Cartridge::numberOfChips()
{
    unsigned result = 0;
    
    for (unsigned i = 0; i < 64; i++)
        if (chip[i] != NULL)
            result++;
    
    return result;
}

unsigned
Cartridge::numberOfBytes()
{
    unsigned result = 0;
    
    for (unsigned i = 0; i < 64; i++)
        if (chip[i] != NULL)
            result += chipSize[i];
    
    return result;
}

void Cartridge::poke(uint16_t addr, uint8_t value)
{
    uint8_t bankNumber;
    
    assert(addr >= 0xDE00 && addr <= 0xDFFF);
    
    // TODO: IMPLEMENT CUSTOM BEHAVIOUR BY SUBCLASSING
    // For some cartridges like Simons basic, bank switching is triggered by writing
    // into I/O area 1 (0xDE00 - 0xDEFF) or I/O area 2 (0xDF00 - 0xDFFF)
    
    // Why do we need to store the written value here?
    rom[addr & 0x7FFF] = value;
    
    switch (type) {
        case CRT_NORMAL:
            break;
            
        case CRT_SIMONS_BASIC:
            if (addr == 0xDE00) {
                // Simon banks the second chip into $A000-BFFF
                if (value == 0x01) {
                    debug(3, "Simons basic: Writing %d into $DE00\n", value);
                    switchBank(1);
                } else {
                    debug(3, "Simons basic: Writing %d into $DE00\n", value);
                    // $A000-BFFF is additional RAM
                    // We need to remove the chip?!
                }
            }
            
        case CRT_C64_GAME_SYSTEM_SYSTEM_3:
            bankNumber = addr - 0xDE00;
            //  Huh? Bank numbers greater than 63 can occur?
            switchBank(bankNumber);
            break;
            
        case CRT_OCEAN_TYPE_1:
            bankNumber = value & 0x3F;
            switchBank(bankNumber);
            break;
        
        default:
            warn("Unsupported cartridge (type %d)\n", type);
    }
}

void
Cartridge::setGameLine(bool value)
{
    gameLine = value;
    c64->mem.updatePeekPokeLookupTables();
}

void
Cartridge::setExromLine(bool value)
{
    exromLine = value;
    c64->mem.updatePeekPokeLookupTables();
}

void
Cartridge::switchBank(unsigned nr)
{
    if (chip[nr] == NULL) {
        warn("Chip %d does not exist (cannot switch)", nr);
        return;
    }
    
    uint16_t loadAddr = chipStartAddress[nr];
    uint16_t size = chipSize[nr];
    
    if (0xFFFF - loadAddr < size) {
        warn("Chip %d covers an invalid memory area (start: %04X size: %d KB)", nr, loadAddr, size / 1024);
        return;
    }
    
    debug(2, "Switching to bank %d (start: %04X size: %d KB)\n", nr, loadAddr, size / 1024);
    memcpy(rom + loadAddr - 0x8000, chip[nr], size);
    for (unsigned i = loadAddr >> 12; i < (loadAddr + size) >> 12; i++) {
        assert (i < 16);
        blendedIn[i] = 1;
    }
}

void
Cartridge::attachChip(unsigned nr, CRTContainer *c)
{
    assert(nr < 64);
    
    if (chip[nr])
        free(chip[nr]);
    
    if (!(chip[nr] = (uint8_t *)malloc(c->getChipSize(nr))))
        return;
    
    chipStartAddress[nr] = c->getChipAddr(nr);
    chipSize[nr] = c->getChipSize(nr);
    memcpy(chip[nr], c->getChipData(nr), c->getChipSize(nr));
    
    debug(1, "Chip %d is in place: %d KB starting at $%04X (type: %d bank:%X)\n",
          nr, chipSize[nr] / 1024, chipStartAddress[nr], c->getChipType(nr), c->getChipBank(nr));
}

bool
Cartridge::attachCartridge(CRTContainer *c)
{
    detachCartridge();
    
    type = c->getCartridgeType();
    gameLine = c->getGameLine();
    exromLine = c->getExromLine();
    
    // Load chip packets
    for (unsigned i = 0; i < c->getNumberOfChips(); i++) {
        attachChip(i, c);
    }
    
    // Hopefully, we got at least one chip
    if(chip[0] == NULL) {
        warn("Cartridge does not contain any chips");
        return false;
    }
    
    // Blend in chip 0
    switchBank(0);
    c64->mem.updatePeekPokeLookupTables();
    // dumpState();
    
    c64->putMessage(MSG_CARTRIDGE, 1);
    return true;
}

void
Cartridge::detachCartridge()
{
    // Deallocate chip memory
    for (unsigned i = 0; i < 64; i++) if (chip[i]) free(chip[i]);
    
    reset();
    if (c64) c64->putMessage(MSG_CARTRIDGE, 0);
}



