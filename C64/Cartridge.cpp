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
    
    // We reset the cartridge here, as C64::reset() keeps the cartridge intact.
    reset();
}

Cartridge::~Cartridge()
{
    debug(1, "  Releasing cartridge...\n");
    
    // Deallocate chip memory
    for (unsigned i = 0; i < 64; i++)
        if (chip[i]) free(chip[i]);
}

bool
Cartridge::isSupportedType(CRTContainer *container)
{
    assert(container != NULL);
    
    CartridgeType type = (CartridgeType)container->getCartridgeType();
    
    switch (type) {
        
        case CRT_NORMAL:
        case CRT_FINAL_CARTRIDGE_III:
        case CRT_SIMONS_BASIC:
        case CRT_OCEAN_TYPE_1:
            return true;
            
        default:
            return false;
    }
}

Cartridge *
Cartridge::makeCartridgeWithCRTContainer(C64 *c64, CRTContainer *container)
{
    assert(isSupportedType(container));
    
    Cartridge *cart;
    // CartridgeType type = container->getCartridgeType();
    
    switch (container->getCartridgeType()) {

        case CRT_NORMAL:
            cart = new Cartridge(c64);
            break;

        case CRT_FINAL_CARTRIDGE_III:
            cart = new Cartridge(c64); // TODO: CartridgeFinalIII
            break;

        case CRT_SIMONS_BASIC:
            cart = new SimonsBasic(c64);
            break;

        case CRT_OCEAN_TYPE_1:
            cart = new OceanType1(c64);
            break;
            

        default:
            assert(false); // should not reach
            return NULL;
    }
    
    assert(cart != NULL);
    
    cart->type = container->getCartridgeType();
    cart->gameLine = container->getGameLine();
    cart->exromLine = container->getExromLine();
    
    // Load chip packets
    for (unsigned i = 0; i < container->getNumberOfChips(); i++) {
        cart->loadChip(i, container);
    }
    
    /*
    // Hopefully, we got at least one chip
    if(cart->chip[0] != NULL) {
        cart->bankIn(0);
    }
    */
    
    return cart;
}

Cartridge *
Cartridge::makeCartridgeWithBuffer(C64 *c64, uint8_t **buffer, CartridgeType type)
{
    Cartridge *cart = new Cartridge(c64);
    if (cart == NULL) return NULL;
    
    cart->type = type;
    cart->loadFromBuffer(buffer);
    
    return cart;
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
Cartridge::powerup()
{
    if (chip[0]) bankIn(0);
    c64->expansionport.gameLineHasChanged();
    c64->expansionport.exromLineHasChanged();
}

void
Cartridge::ping()
{
}

uint32_t
Cartridge::stateSize()
{
    uint32_t size = 2;
    
    for (unsigned i = 0; i < 64; i++) {
        size += 4 + chipSize[i];
    }

    size += sizeof(rom);
    size += sizeof(blendedIn);
    
    return size;
}

void
Cartridge::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
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
    msg("Cartridge (class Cartridge)\n");
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

uint8_t
Cartridge::peek(uint16_t addr)
{
    return rom[addr & 0x7FFF];
    
    /*
    if (romIsBlendedIn(addr)) {
        return rom[addr & 0x7FFF];
    } else {
        // Question: What ist the correct default behavior here?
        debug("Returning value from ROM");
        return c64->mem.rom[addr];
    }
    */
}

void
Cartridge::poke(uint16_t addr, uint8_t value)
{
    uint8_t bankNumber;
    
    assert(addr >= 0xDE00 && addr <= 0xDFFF);
    
     // Why do we need to store the written value here?
    rom[addr & 0x7FFF] = value;
    
    switch (type) {
        case CRT_NORMAL:
            break;
            
        case CRT_SIMONS_BASIC:
            assert(0);
  
        case CRT_C64_GAME_SYSTEM_SYSTEM_3:
            bankNumber = addr - 0xDE00;
            //  Huh? Bank numbers greater than 63 can occur?
            bankIn(bankNumber);
            break;
        
        default:
            warn("Unsupported cartridge (type %d)\n", type);
            assert(0);
    }
}

void
Cartridge::setGameLine(bool value)
{

    gameLine = value;
    c64->expansionport.gameLineHasChanged();
}

void
Cartridge::setExromLine(bool value)
{
    exromLine = value;
    c64->expansionport.exromLineHasChanged();
}

/*
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
*/

void
Cartridge::bankIn(unsigned nr)
{
    assert(nr < 64);
    assert(chip[nr] != NULL);

    uint16_t start = chipStartAddress[nr];
    uint16_t size  = chipSize[nr];
    uint16_t end   = start + size;
    assert(0xFFFF - start >= size);

    memcpy(rom + start - 0x8000, chip[nr], size);
    for (unsigned i = start >> 12; i < end >> 12; i++)
        blendedIn[i] = 1;

    debug(1, "Chip %d banked in (start: %04X size: %d KB)\n", nr, start, size / 1024);
    
    for (unsigned i = 0; i < 16; i++) {
        printf("%d ", blendedIn[i]);
    }
    printf("\n");
}

void
Cartridge::bankOut(unsigned nr)
{
    assert(nr < 64);
    assert(chip[nr] != NULL);

    uint16_t start = chipStartAddress[nr];
    uint16_t size  = chipSize[nr];
    uint16_t end   = start + size;
    assert(0xFFFF - start >= size);
    
    for (unsigned i = start >> 12; i < end >> 12; i++)
        blendedIn[i] = 0;
    
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
    
    if (0xFFFF - start < size) {
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



