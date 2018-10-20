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
    debug("  Creating cartridge at address %p...\n", this);

    this->c64 = c64;
    
    initialGameLine = 1;
    initialExromLine = 1;
    
    for (unsigned i = 0; i < 64; i++) {
        chip[i] = NULL;
        chipStartAddress[i] = 0;
        chipSize[i] = 0;
    }
    chipL = chipH = 0;
    offsetL = offsetH = 0;
    mappedBytesL = mappedBytesH = 0;
    
    externalRam = NULL;
    ramCapacity = 0;
    persistentRam = false;
    
    cycle = 0;
    regValue = 0;
}

Cartridge::~Cartridge()
{
    debug(1, "  Releasing cartridge...\n");
    
    // Deallocate ROM chips
    for (unsigned i = 0; i < 64; i++)
        if (chip[i]) free(chip[i]);
    
    // Deallocate RAM (if any)
    if (externalRam) {
        assert(ramCapacity > 0);
        free(externalRam);
    }
}

void
Cartridge::reset()
{
    // Delete RAM
    if (externalRam && !persistentRam) {
        memset(externalRam, 0, ramCapacity);
    }
    
    // Bank in visibile chips (chips with low numbers show up first)
    for (int i = 63; i >= 0; i--) {
        bankIn(i);
    }
    
    cycle = 0;
    regValue = 0;
}

bool
Cartridge::isSupportedType(CartridgeType type)
{    
    switch (type) {
        
        case CRT_NORMAL:
        case CRT_ACTION_REPLAY:
        case CRT_KCS_POWER:
        case CRT_FINAL_III:
        case CRT_SIMONS_BASIC:
        case CRT_OCEAN:
            
        case CRT_FUNPLAY:
        case CRT_SUPER_GAMES:
        case CRT_EPYX_FASTLOAD:
        case CRT_WESTERMANN:
        case CRT_REX:
            
        case CRT_WARPSPEED:
            
        case CRT_ZAXXON:
        case CRT_MAGIC_DESK:
            
        case CRT_COMAL80:
            
        case CRT_ACTION_REPLAY3:
            
        case CRT_FREEZE_FRAME:
            
        case CRT_GEO_RAM:
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
        case CRT_ACTION_REPLAY:
            return new ActionReplay(c64);
        case CRT_KCS_POWER:
            return new KcsPower(c64); 
        case CRT_FINAL_III:
            return new FinalIII(c64);
        case CRT_SIMONS_BASIC:
            return new SimonsBasic(c64);
        case CRT_OCEAN:
            return new Ocean(c64);
        case CRT_FUNPLAY:
            return new Funplay(c64);
        case CRT_SUPER_GAMES:
            return new Supergames(c64);
        case CRT_EPYX_FASTLOAD:
            return new EpyxFastLoad(c64);
        case CRT_WESTERMANN:
            return new Westermann(c64);
        case CRT_REX:
            return new Rex(c64);
        case CRT_WARPSPEED:
            return new WarpSpeed(c64);
        case CRT_ZAXXON:
            return new Zaxxon(c64);
        case CRT_MAGIC_DESK:
            return new MagicDesk(c64);
        case CRT_COMAL80:
            return new Comal80(c64);
        case CRT_ACTION_REPLAY3:
            return new ActionReplay3(c64);
        case CRT_FREEZE_FRAME:
            return new FreezeFrame(c64);
        case CRT_GEO_RAM:
            return new GeoRAM(c64);
            
        default:
            assert(false); // should not reach
            return NULL;
    }
}

Cartridge *
Cartridge::makeCartridgeWithCRTContainer(C64 *c64, CRTFile *container)
{
    Cartridge *cart;
    
    cart = makeCartridgeWithType(c64, container->cartridgeType());
    assert(cart != NULL);
    
    // Remember powerup values for game line and exrom line
    cart->initialGameLine  = container->gameLine();
    cart->initialExromLine = container->exromLine();
    
    // Load chip packets
    for (unsigned i = 0; i < container->chipCount(); i++) {
        cart->loadChip(i, container);
    }
    
    return cart;
}

size_t
Cartridge::stateSize()
{
    uint32_t size = 0;

    size += 1; // initialGameLine
    size += 1; // initialExromLine

    for (unsigned i = 0; i < 64; i++) {
        size += 4 + chipSize[i];
    }
    size += sizeof(chipL);
    size += sizeof(chipH);
    size += sizeof(mappedBytesL);
    size += sizeof(mappedBytesH);
    size += sizeof(offsetL);
    size += sizeof(offsetH);

    size += sizeof(ramCapacity);
    size += ramCapacity;
    size += 1; // persistentRam

    // size += sizeof(blendedIn);
    size += sizeof(cycle);
    size += sizeof(regValue);

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
            if (chip[i] != NULL) free(chip[i]);
            chip[i] = (uint8_t *)malloc(chipSize[i]);
            readBlock(buffer, chip[i], chipSize[i]);
        } else {
            chip[i] = NULL;
        }
    }
    chipL = read8(buffer);
    chipH = read8(buffer);
    mappedBytesL = read16(buffer);
    mappedBytesH = read16(buffer);
    offsetL = read16(buffer);
    offsetH = read16(buffer);
    
    setRamCapacity(read32(buffer));
    readBlock(buffer, externalRam, ramCapacity);
    persistentRam = (bool)read8(buffer);
    
    // readBlock(buffer, blendedIn, sizeof(blendedIn));
    cycle = read64(buffer);
    regValue = read8(buffer);

    debug(2, "  Cartridge state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == Cartridge::stateSize());
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
    write8(buffer, chipL);
    write8(buffer, chipH);
    write16(buffer, mappedBytesL);
    write16(buffer, mappedBytesH);
    write16(buffer, offsetL);
    write16(buffer, offsetH);
    
    write32(buffer, ramCapacity);
    writeBlock(buffer, externalRam, ramCapacity);
    write8(buffer, (uint8_t)persistentRam);
    
    // writeBlock(buffer, blendedIn, sizeof(blendedIn));
    write64(buffer, cycle);
    write8(buffer, regValue);

    debug(4, "  Cartridge state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == Cartridge::stateSize());
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
    assert(isROMLaddr(addr) || isROMHaddr(addr));

    uint16_t relAddr = addr & 0x1FFF;

    // Question: Is it correct to return a value from RAM if no ROM is mapped?
    if (isROMLaddr(addr)) {
        return (relAddr < mappedBytesL) ? peekRomL(relAddr) : c64->mem.ram[addr];
    } else {
        return (relAddr < mappedBytesH) ? peekRomH(relAddr) : c64->mem.ram[addr];
    }
}

uint8_t
Cartridge::peekRomL(uint16_t addr)
{
    assert(addr <= 0x1FFF);
    assert(chipL >= 0 && chipL < 64);
    assert(addr + offsetL < chipSize[chipL]);
    
    return chip[chipL][addr + offsetL];
}

uint8_t
Cartridge::peekRomH(uint16_t addr)
{
    assert(addr <= 0x1FFF);
    assert(chipH >= 0 && chipH < 64);
    assert(addr + offsetH < chipSize[chipH]);
    
    return chip[chipH][addr + offsetH];
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

uint32_t
Cartridge::getRamCapacity()
{
    if (ramCapacity == 0) {
        assert(externalRam == NULL);
    } else {
        assert(externalRam != NULL);
    }
    return ramCapacity;
}

void
Cartridge::setRamCapacity(uint32_t size)
{
    // Free
    if (ramCapacity != 0 || externalRam != NULL) {
        assert(ramCapacity > 0 && externalRam != NULL);
        free(externalRam);
        externalRam = NULL;
        ramCapacity = 0;
    }
    
    // Allocate
    if (size > 0) {
        externalRam = (uint8_t *)malloc((size_t)size);
        ramCapacity = size;
        memset(externalRam, 0, size);
    }
}

void
Cartridge::loadChip(unsigned nr, CRTFile *c)
{
    assert(nr < 64);
    assert(c != NULL);
    
    uint16_t start = c->chipAddr(nr);
    uint16_t size  = c->chipSize(nr);
    uint8_t  *data = c->chipData(nr);
    
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
}

bool
Cartridge::mapsToL(unsigned nr) {
    assert(nr < 64);
    return chipStartAddress[nr] == 0x8000 && chipSize[nr] <= 0x2000;
}

bool
Cartridge::mapsToLH(unsigned nr) {
    assert(nr < 64);
    return chipStartAddress[nr] == 0x8000 && chipSize[nr] > 0x2000;
}

bool
Cartridge::mapsToH(unsigned nr) {
    assert(nr < 64);
    return chipStartAddress[nr] == 0xA000 || chipStartAddress[nr] == 0xE000;
}

void
Cartridge::bankInROML(unsigned nr, uint16_t size, uint16_t offset)
{
    chipL = nr;
    mappedBytesL = size;
    offsetL = offset;
}

void
Cartridge::bankInROMH(unsigned nr, uint16_t size, uint16_t offset)
{
    chipH = nr;
    mappedBytesH = size;
    offsetH = offset;
}

void
Cartridge::bankIn(unsigned nr)
{
    assert(nr < 64);
    assert(chipSize[nr] <= 0x4000);
    
    if (chip[nr] == NULL)
        return;

    if (mapsToLH(nr)) {
        
        bankInROML(nr, 0x2000, 0); // chip covers ROML and (part of) ROMH
        bankInROMH(nr, chipSize[nr] - 0x2000, 0x2000);
        debug(2, "Banked in chip %d in ROML and ROMH\n", nr);
    
    } else if (mapsToL(nr)) {
        
        bankInROML(nr, chipSize[nr], 0); // chip covers (part of) ROML
        debug(2, "Banked in chip %d in ROML\n", nr);
        
    } else if (mapsToH(nr)) {
        
        bankInROMH(nr, chipSize[nr], 0); // chip covers (part of) ROMH
        debug(2, "Banked in chip %d to ROMH\n", nr);
        
    } else {

        warn("Cannot map chip %d. Invalid start address.\n", nr);
    }
}

/*
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
*/

void
Cartridge::bankOut(unsigned nr)
{
    assert(nr < 64);

    if (mapsToL(nr)) {
        
        chipL = -1;
        mappedBytesL = 0;
        offsetL = 0;
        
    } else if (mapsToH(nr)) {
        
        chipH = -1;
        mappedBytesH = 0;
        offsetH = 0;
    }
}

void
Cartridge::pressResetButton()
{
    // Reset all components, but keep memory contents
    uint8_t ram[0xFFFF];
    
    c64->suspend();
    memcpy(ram, c64->mem.ram, 0xFFFF);
    c64->reset();
    memcpy(c64->mem.ram, ram, 0xFFFF);
    c64->resume();
}
