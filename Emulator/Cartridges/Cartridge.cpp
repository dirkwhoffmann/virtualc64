// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

Cartridge::Cartridge(C64 *c64, C64 &ref, const char *description) : C64Component(ref)
{
    setDescription(description);
    debug(CRT_DEBUG, "Creating cartridge at address %p...\n", this);
    
    memset(packet, 0, sizeof(packet));
    
    SnapshotItem items[] = {
        
        // Configuration items
        { &gameLineInCrtFile,  sizeof(gameLineInCrtFile),  KEEP_ON_RESET },
        { &exromLineInCrtFile, sizeof(exromLineInCrtFile), KEEP_ON_RESET },
        { &numPackets,         sizeof(numPackets),         KEEP_ON_RESET },
        
        { &chipL,              sizeof(chipL),              CLEAR_ON_RESET },
        { &chipH,              sizeof(chipH),              CLEAR_ON_RESET },
        { &mappedBytesL,       sizeof(mappedBytesL),       CLEAR_ON_RESET },
        { &mappedBytesH,       sizeof(mappedBytesH),       CLEAR_ON_RESET },
        { &offsetL,            sizeof(offsetL),            CLEAR_ON_RESET },
        { &offsetH,            sizeof(offsetH),            CLEAR_ON_RESET },

        { &ramCapacity,        sizeof(ramCapacity),        KEEP_ON_RESET },
        { &persistentRam,      sizeof(persistentRam),      KEEP_ON_RESET },

        { &switchPos,          sizeof(switchPos),          KEEP_ON_RESET },
        { &led,                sizeof(led),                CLEAR_ON_RESET },
     
        { NULL,                0,                          0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

Cartridge::~Cartridge()
{
    debug(CRT_DEBUG, "Releasing cartridge...\n");
    
    // Deallocate RAM (if any)
    if (externalRam) {
        assert(ramCapacity > 0);
        free(externalRam);
    }
}

void
Cartridge::dealloc()
{
    for (unsigned i = 0; i < numPackets; i++) {
        assert(packet[i] != NULL);
        delete packet[i];
        packet[i] = NULL;
    }
    
    numPackets = 0;
}

void
Cartridge::oldReset()
{
    // Reset external RAM
    if (externalRam && !persistentRam) {
        memset(externalRam, 0xFF, ramCapacity);
    }
 
    // Reset all chip packets
    for (unsigned i = 0; i < numPackets; i++) {
        packet[i]->oldReset();
    }
        
    // Bank in visibile chips (chips with low numbers show up first)
    for (int i = MAX_PACKETS - 1; i >= 0; i--) {
        bankIn(i);
    }
}

void
Cartridge::resetCartConfig() {

    expansionport.setGameAndExrom(gameLineInCrtFile, exromLineInCrtFile);
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
        case CRT_EXPERT:
        case CRT_FUNPLAY:
        case CRT_SUPER_GAMES:
        case CRT_ATOMIC_POWER:
        case CRT_EPYX_FASTLOAD:
        case CRT_WESTERMANN:
        case CRT_REX:
            
        case CRT_WARPSPEED:
            
        case CRT_ZAXXON:
        case CRT_MAGIC_DESK:
            
        case CRT_COMAL80:

        case CRT_MIKRO_ASS:

        case CRT_STARDOS:
        case CRT_EASYFLASH:
            
        case CRT_ACTION_REPLAY3:
            
        case CRT_FREEZE_FRAME:

        case CRT_MACH5:
            
        case CRT_KINGSOFT:
            
        case CRT_ISEPIC:
        case CRT_GEO_RAM:
            return true;
            
        default:
            return false;
    }
}

Cartridge *
Cartridge::makeWithType(C64 *c64, CartridgeType type)
{
    assert(isSupportedType(type));
    
    switch (type) {
        
        case CRT_NORMAL:          return new Cartridge(c64, *c64);
        case CRT_ACTION_REPLAY:   return new ActionReplay(c64, *c64);
        case CRT_KCS_POWER:       return new KcsPower(c64, *c64);
        case CRT_FINAL_III:       return new FinalIII(c64, *c64);
        case CRT_SIMONS_BASIC:    return new SimonsBasic(c64, *c64);
        case CRT_OCEAN:           return new Ocean(c64, *c64);
        case CRT_EXPERT:          return new Expert(c64, *c64);
        case CRT_FUNPLAY:         return new Funplay(c64, *c64);
        case CRT_SUPER_GAMES:     return new Supergames(c64, *c64);
        case CRT_ATOMIC_POWER:    return new AtomicPower(c64, *c64);
        case CRT_EPYX_FASTLOAD:   return new EpyxFastLoad(c64, *c64);
        case CRT_WESTERMANN:      return new Westermann(c64, *c64);
        case CRT_REX:             return new Rex(c64, *c64);
        case CRT_WARPSPEED:       return new WarpSpeed(c64, *c64);
        case CRT_ZAXXON:          return new Zaxxon(c64, *c64);
        case CRT_MAGIC_DESK:      return new MagicDesk(c64, *c64);
        case CRT_COMAL80:         return new Comal80(c64, *c64);
        case CRT_MIKRO_ASS:       return new MikroAss(c64, *c64);
        case CRT_STARDOS:         return new StarDos(c64, *c64);
        case CRT_EASYFLASH:       return new EasyFlash(c64, *c64);
        case CRT_ACTION_REPLAY3:  return new ActionReplay3(c64, *c64);
        case CRT_FREEZE_FRAME:    return new FreezeFrame(c64, *c64);
        case CRT_MACH5:           return new Mach5(c64, *c64);
        case CRT_KINGSOFT:        return new Kingsoft(c64, *c64);
        case CRT_ISEPIC:          return new Isepic(c64, *c64);
        case CRT_GEO_RAM:         return new GeoRAM(c64, *c64);
        
        default:
        assert(false); // Should not reach
        return NULL;
    }
}

Cartridge *
Cartridge::makeWithCRTFile(C64 *c64, CRTFile *file)
{
    Cartridge *cart;
    
    cart = makeWithType(c64, file->cartridgeType());
    assert(cart != NULL);
    
    // Remember powerup values for game line and exrom line
    cart->gameLineInCrtFile = file->initialGameLine();
    cart->exromLineInCrtFile = file->initialExromLine();

    // Load chip packets
    cart->numPackets = 0;
    for (unsigned i = 0; i < file->chipCount(); i++) {
        cart->loadChip(i, file);
    }
    
    return cart;
}

size_t
Cartridge::packetStateSize()
{
    size_t result = 0;
    
    for (unsigned i = 0; i < numPackets; i++) {
        assert(packet[i] != NULL);
        result += packet[i]->stateSize();
    }
    
    return result;
}

void
Cartridge::loadPacketsFromBuffer(u8 **buffer)
{
    for (unsigned i = 0; i < numPackets; i++) {
        assert(packet[i] == NULL);
        packet[i] = new CartridgeRom(vc64);
        packet[i]->loadFromBuffer(buffer);
    }
}

void
Cartridge::savePacketsToBuffer(u8 **buffer)
{
    for (unsigned i = 0; i < numPackets; i++) {
        assert(packet[i] != NULL);
        packet[i]->saveToBuffer(buffer);
    }
}

size_t
Cartridge::stateSize()
{
    return HardwareComponent::stateSize()
    + packetStateSize()
    + ramCapacity;
}

void
Cartridge::didLoadFromBuffer(u8 **buffer)
{
    setRamCapacity(ramCapacity);
    
    loadPacketsFromBuffer(buffer);
    readBlock(buffer, externalRam, ramCapacity);
}

void
Cartridge::didSaveToBuffer(u8 **buffer)
{
    savePacketsToBuffer(buffer);
    writeBlock(buffer, externalRam, ramCapacity);
}

void
Cartridge::_dump()
{
    msg("\n");
    msg("Cartridge\n");
    msg("---------\n");
    
    msg("        Cartridge type: %d\n", getCartridgeType());
    msg(" Game line in CRT file: %d\n", gameLineInCrtFile);
    msg("Exrom line in CRT file: %d\n", exromLineInCrtFile);
    msg(" Number of Rom packets: %d\n", numPackets);
    
    for (unsigned i = 0; i < numPackets; i++) {
        msg("              Chip %3d: %d KB starting at $%04X\n",
            i, packet[i]->size / 1024, packet[i]->loadAddress);
    }
    msg("\n");
}

u8
Cartridge::peek(u16 addr)
{
    assert(isROMLaddr(addr) || isROMHaddr(addr));

    u16 relAddr = addr & 0x1FFF;

    // Question: Is it correct to return a value from RAM if no ROM is mapped?
    if (isROMLaddr(addr)) {
        return (relAddr < mappedBytesL) ? peekRomL(relAddr) : mem.ram[addr];
    } else {
        return (relAddr < mappedBytesH) ? peekRomH(relAddr) : mem.ram[addr];
    }
}

u8
Cartridge::peekRomL(u16 addr)
{
    assert(addr <= 0x1FFF);
    assert(chipL >= 0 && chipL < numPackets);
    
    return packet[chipL]->peek(addr + offsetL);
}

u8
Cartridge::peekRomH(u16 addr)
{
    assert(addr <= 0x1FFF);
    assert(chipH >= 0 && chipH < numPackets);
    
    return packet[chipH]->peek(addr + offsetH);
}

void
Cartridge::poke(u16 addr, u8 value)
{
    assert(isROMLaddr(addr) || isROMHaddr(addr));
    
    u16 relAddr = addr & 0x1FFF;
    
    if (isROMLaddr(addr) && relAddr < mappedBytesL) {
        pokeRomL(relAddr, value);
    }
    if (isROMHaddr(addr) && relAddr < mappedBytesH) {
        pokeRomH(relAddr, value);
    }
        
    // Write to RAM if we don't run in Ultimax mode
    if (!vc64.getUltimax()) {
        mem.ram[addr] = value;
    }
}

u32
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
Cartridge::setRamCapacity(u32 size)
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
        externalRam = (u8 *)malloc((size_t)size);
        ramCapacity = size;
        memset(externalRam, 0xFF, size);
    }
}

void
Cartridge::loadChip(unsigned nr, CRTFile *c)
{
    assert(nr < MAX_PACKETS);
    assert(c != NULL);
    
    u16 size = c->chipSize(nr);
    u16 start = c->chipAddr(nr);
    u16 type = c->chipType(nr);
    
    // Perform some consistency checks
    if (start < 0x8000) {
        warn("Ignoring chip %d: Start address too low (%04X)\n", nr, start);
        return;
    }
    if (0x10000 - start < size) {
        warn("Ignoring chip %d: Invalid size (start: %04X size: %04X)/n", nr, start, size);
        return;
    }
    
    // Delete old chip packet if present
    if (packet[nr]) {
        delete packet[nr];
    }
    
    // Create new chip packet
    switch (type) {
        
        case 0: // ROM
        packet[nr] = new CartridgeRom(vc64, size, start, c->chipData(nr));
        break;
        
        case 1: // RAM
        warn("Ignoring chip %d, because it has type RAM.\n", nr);
        return;
        
        case 2: // Flash ROM
        warn("Chip %d is a Flash Rom. Creating a Rom instead.\n", nr);
        packet[nr] = new CartridgeRom(vc64, size, start, c->chipData(nr));
        break;
        
        default:
        warn("Ignoring chip %d, because it has unknown type %d.\n", nr, type);
        return;
    }
    
    numPackets++;
}

void
Cartridge::bankInROML(unsigned nr, u16 size, u16 offset)
{
    chipL = nr;
    mappedBytesL = size;
    offsetL = offset;
}

void
Cartridge::bankInROMH(unsigned nr, u16 size, u16 offset)
{
    chipH = nr;
    mappedBytesH = size;
    offsetH = offset;
}

void
Cartridge::bankIn(unsigned nr)
{
    assert(nr < MAX_PACKETS);
    
    if (packet[nr] == NULL)
        return;

    assert(packet[nr]->size <= 0x4000);

    if (packet[nr]->mapsToLH()) {
        
        bankInROML(nr, 0x2000, 0); // chip covers ROML and (part of) ROMH
        bankInROMH(nr, packet[nr]->size - 0x2000, 0x2000);
        debug(CRT_DEBUG, "Banked in chip %d in ROML and ROMH\n", nr);
    
    } else if (packet[nr]->mapsToL()) {
        
        bankInROML(nr, packet[nr]->size, 0); // chip covers (part of) ROML
        debug(CRT_DEBUG, "Banked in chip %d in ROML\n", nr);
        
    } else if (packet[nr]->mapsToH()) {
        
        bankInROMH(nr, packet[nr]->size, 0); // chip covers (part of) ROMH
        debug(CRT_DEBUG, "Banked in chip %d to ROMH\n", nr);
        
    } else {

        warn("Cannot map chip %d. Invalid start address.\n", nr);
    }
}

void
Cartridge::bankOut(unsigned nr)
{
    assert(nr < MAX_PACKETS);

    if (packet[nr]->mapsToL()) {
        
        chipL = -1;
        mappedBytesL = 0;
        offsetL = 0;
        
    } else if (packet[nr]->mapsToH()) {
        
        chipH = -1;
        mappedBytesH = 0;
        offsetH = 0;
    }
}

void
Cartridge::setSwitch(i8 pos)
{
    switchPos = pos;
    vc64.putMessage(MSG_CART_SWITCH);
}

void
Cartridge::resetWithoutDeletingRam()
{
    u8 ram[0x10000];
    
    debug(RUN_DEBUG, "Resetting virtual C64 (preserving RAM)\n");
    
    memcpy(ram, mem.ram, 0x10000);
    vc64.oldReset();
    memcpy(mem.ram, ram, 0x10000);
}
