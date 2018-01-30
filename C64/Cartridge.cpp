//
//  Cartridge.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 20.01.18.
//

#include "C64.h"

Cartridge::Cartridge(C64 *c64)
{
    setDescription("Cartridge");
    debug(1, "  Creating cartridge at address %p...\n", this);

    this->c64 = c64;
    
    initialGameLine = 1;
    initialExromLine = 1;
    memset(blendedIn, 255, sizeof(blendedIn));
    
    for (unsigned i = 0; i < 64; i++) {
        chip[i] = NULL;
        chipStartAddress[i] = 0;
        chipSize[i] = 0;
    }
}

Cartridge::~Cartridge()
{
    debug(1, "  Releasing cartridge...\n");
    
    // Deallocate chip memory
    for (unsigned i = 0; i < 64; i++)
        if (chip[i]) free(chip[i]);
}

void
Cartridge::reset()
{
    // Bank in chip 0 on startup
    bankIn(0);
}

bool
Cartridge::isSupportedType(CartridgeType type)
{    
    switch (type) {
        
        case CRT_NORMAL:
        case CRT_FINAL_CARTRIDGE_III:
        case CRT_SIMONS_BASIC:
        case CRT_OCEAN_TYPE_1:
        case CRT_FUN_PLAY_POWER_PLAY:
        case CRT_SUPER_GAMES:
        case CRT_EPYX_FASTLOAD:
        case CRT_WESTERMANN_LEARNING:
            return true;
            
        default:
            return false;
    }
}

Cartridge *
Cartridge::makeCartridgeWithType(C64 *c64, CartridgeType type)
{
     assert(isSupportedType(type));
    
    switch (type) {
            
        case CRT_NORMAL:
            return new Cartridge(c64);
            
        case CRT_FINAL_CARTRIDGE_III:
            return new FinalIII(c64);
            
        case CRT_SIMONS_BASIC:
            return new SimonsBasic(c64);
            
        case CRT_OCEAN_TYPE_1:
            return new OceanType1(c64);
            
        case CRT_FUN_PLAY_POWER_PLAY:
            return new Powerplay(c64);
        
        case CRT_SUPER_GAMES:
            return new Supergames(c64);

        case CRT_EPYX_FASTLOAD:
            return new EpyxFastLoad(c64);
            
        case CRT_WESTERMANN_LEARNING:
            return new Westermann(c64);

        default:
            assert(false); // should not reach
            return NULL;
    }
}

Cartridge *
Cartridge::makeCartridgeWithCRTContainer(C64 *c64, CRTContainer *container)
{
    Cartridge *cart;
    
    cart = makeCartridgeWithType(c64, container->getCartridgeType());
    assert(cart != NULL);
    
    // Remember powerup values for game line and exrom line
    cart->initialGameLine  = container->getGameLine();
    cart->initialExromLine = container->getExromLine();
    
    // Load chip packets
    for (unsigned i = 0; i < container->getNumberOfChips(); i++) {
        cart->loadChip(i, container);
    }
    
    return cart;
}

uint32_t
Cartridge::stateSize()
{
    uint32_t size = 2;
    
    for (unsigned i = 0; i < 64; i++) {
        size += 4 + chipSize[i];
    }

    size += sizeof(blendedIn);
    
    return size;
}

void
Cartridge::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    initialGameLine = (bool)read8(buffer);
    initialExromLine = (bool)read8(buffer);
    
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
    
    readBlock(buffer, blendedIn, sizeof(blendedIn));
    
    debug(2, "  Cartridge state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
Cartridge::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    write8(buffer, (uint8_t)initialGameLine);
    write8(buffer, (uint8_t)initialExromLine);
    
    for (unsigned i = 0; i < 64; i++) {
        write16(buffer, chipStartAddress[i]);
        write16(buffer, chipSize[i]);
        
        if (chipSize[i] > 0) {
            writeBlock(buffer, chip[i], chipSize[i]);
        }
    }
    
    writeBlock(buffer, blendedIn, sizeof(blendedIn));
    
    debug(4, "  Cartridge state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
Cartridge::dumpState()
{
    msg("\n");
    msg("Cartridge\n");
    msg("---------\n");
    
    msg("Cartridge type:     %d\n", getCartridgeType());
    msg("Initial game line:  %d\n", initialGameLine);
    msg("Initial exrom line: %d\n", initialExromLine);
    
    for (unsigned i = 0; i < 64; i++) {
        if (chip[i] != NULL) {
            msg("Chip %2d:        %d KB starting at $%04X\n", i, chipSize[i] / 1024, chipStartAddress[i]);
        }
    }
}

uint8_t
Cartridge::peek(uint16_t addr)
{
    uint8_t bank = addr / 0x1000;
    uint8_t nr   = blendedIn[bank];
    
    if (nr < 64) {
        
        assert(chip[nr] != NULL);

        uint16_t offset = addr - chipStartAddress[nr];
        assert(offset < chipSize[nr]);
        
        return chip[nr][offset];
    }
    
    // No cartridge chip is mapped to this memory area
    // debug("Peeking from unmapped location: %04X\n", addr);
    return c64->mem.peekRam(addr);
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

void
Cartridge::bankIn(unsigned nr)
{
    assert(nr < 64);
    
    if (chip[nr] == NULL)
        return;

    uint16_t start     = chipStartAddress[nr];
    uint16_t size      = chipSize[nr];
    uint8_t  firstBank = start / 0x1000;
    uint8_t  numBanks  = size / 0x1000;
    assert (firstBank + numBanks <= 16);

    for (unsigned i = 0; i < numBanks; i++)
        blendedIn[firstBank + i] = nr;
    
    /*
    debug(1, "Chip %d banked in (start: %04X size: %d KB)\n", nr, start, size / 1024);
    for (unsigned i = 0; i < 16; i++) {
        printf("%d ", blendedIn[i]);
    }
    printf("\n");
    */
}

void
Cartridge::bankOut(unsigned nr)
{
    assert(nr < 64);
    assert(chip[nr] != NULL);

    uint16_t start     = chipStartAddress[nr];
    uint16_t size      = chipSize[nr];
    uint8_t  firstBank = start / 0x1000;
    uint8_t  numBanks  = size / 0x1000;
    assert (firstBank + numBanks <= 16);
    
    for (unsigned i = 0; i < numBanks; i++)
        blendedIn[firstBank + i] = 255;
    
    debug(1, "Chip %d banked out (start: %04X size: %d KB)\n", nr, start, size / 1024);
    for (unsigned i = 0; i < 16; i++) {
        printf("%d ", blendedIn[i]);
    }
    printf("\n");
}

void
Cartridge::loadChip(unsigned nr, CRTContainer *c)
{
    assert(nr < 64);
    assert(c != NULL);
    
    uint16_t start = c->getChipAddr(nr);
    uint16_t size  = c->getChipSize(nr);
    uint8_t  *data = c->getChipData(nr);
    
    if (start < 0x8000) {
        warn("Ignoring chip %d: Start address too low (%04X)", nr, start);
        return;
    }
    
    if (0x10000 - start < size) {
        warn("Ignoring chip %d: Invalid size (start: %04X size: %04X)", nr, start, size);
        return;
    }
    
    if (chip[nr])
        free(chip[nr]);
    
    if (!(chip[nr] = (uint8_t *)malloc(size)))
        return;
    
    chipStartAddress[nr] = start;
    chipSize[nr]         = size;
    memcpy(chip[nr], data, size);
    
    /*
    debug(1, "Chip %d is in place: %d KB starting at $%04X (type: %d bank:%X)\n",
          nr, chipSize[nr] / 1024, chipStartAddress[nr], c->getChipType(nr), c->getChipBank(nr));
    */
}




