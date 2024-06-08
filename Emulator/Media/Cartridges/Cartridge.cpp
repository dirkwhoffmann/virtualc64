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

#include "config.h"
#include "Cartridge.h"
#include "C64.h"

namespace vc64 {

bool
Cartridge::isKnownType(CartridgeType type)
{
    if (FORCE_CRT_UNKNOWN) return false;

    return type >= CRT_NORMAL && type <= CRT_GMOD2;
}

bool
Cartridge::isSupportedType(CartridgeType type)
{
    if (FORCE_CRT_UNSUPPORTED) return false;

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
        case CRT_DINAMIC:
        case CRT_ZAXXON:
        case CRT_MAGIC_DESK:

        case CRT_COMAL80:
        case CRT_STRUCTURED_BASIC:

        case CRT_MIKRO_ASSEMBLER:

        case CRT_STARDOS:
        case CRT_EASYFLASH:

        case CRT_ACTION_REPLAY3:

        case CRT_GAME_KILLER:

        case CRT_FREEZE_FRAME:

        case CRT_MACH5:

        case CRT_PAGEFOX:
        case CRT_KINGSOFT:
        case CRT_GMOD2:


        case CRT_ISEPIC:
        case CRT_GEO_RAM:
        case CRT_REU:

            return true;

        default:
            break;
    }

    return false;
}

bool
Cartridge::isROMLaddr (u16 addr)
{
    // ROML is mapped to 0x8000 - 0x9FFF
    return addr >= 0x8000 && addr <= 0x9FFF;
}

bool
Cartridge::isROMHaddr (u16 addr)
{
    // ROMH is mapped to 0xA000 - 0xBFFF or 0xE000 - 0xFFFF
    return (addr >= 0xA000 && addr <= 0xBFFF) || (addr >= 0xE000 && addr <= 0xFFFF);
}

Cartridge *
Cartridge::makeWithType(C64 &c64, CartridgeType type)
{
    Cartridge *cart;

    switch (type) {

        case CRT_NONE:              return nullptr;
            
        case CRT_NORMAL:            cart = new Cartridge(c64); break;
        case CRT_ACTION_REPLAY:     cart = new ActionReplay(c64); break;
        case CRT_KCS_POWER:         cart = new KcsPower(c64); break;
        case CRT_FINAL_III:         cart = new FinalIII(c64); break;
        case CRT_SIMONS_BASIC:      cart = new SimonsBasic(c64); break;
        case CRT_OCEAN:             cart = new Ocean(c64); break;
        case CRT_EXPERT:            cart = new Expert(c64); break;
        case CRT_FUNPLAY:           cart = new Funplay(c64); break;
        case CRT_SUPER_GAMES:       cart = new SuperGames(c64); break;
        case CRT_ATOMIC_POWER:      cart = new AtomicPower(c64); break;
        case CRT_EPYX_FASTLOAD:     cart = new Epyx(c64); break;
        case CRT_WESTERMANN:        cart = new Westermann(c64); break;
        case CRT_REX:               cart = new Rex(c64); break;
        case CRT_WARPSPEED:         cart = new WarpSpeed(c64); break;
        case CRT_DINAMIC:           cart = new Dinamic(c64); break;
        case CRT_ZAXXON:            cart = new Zaxxon(c64); break;
        case CRT_MAGIC_DESK:        cart = new MagicDesk(c64); break;
        case CRT_COMAL80:           cart = new Comal80(c64); break;
        case CRT_STRUCTURED_BASIC:  cart = new StructuredBasic(c64); break;
        case CRT_MIKRO_ASSEMBLER:   cart = new MikroAss(c64); break;
        case CRT_STARDOS:           cart = new StarDos(c64); break;
        case CRT_EASYFLASH:         cart = new EasyFlash(c64); break;
        case CRT_ACTION_REPLAY3:    cart = new ActionReplay3(c64); break;
        case CRT_GAME_KILLER:       cart = new GameKiller(c64); break;
        case CRT_FREEZE_FRAME:      cart = new FreezeFrame(c64); break;
        case CRT_MACH5:             cart = new Mach5(c64); break;
        case CRT_PAGEFOX:           cart = new PageFox(c64); break;
        case CRT_KINGSOFT:          cart = new Kingsoft(c64); break;
        case CRT_GMOD2:             cart = new Gmod2(c64); break;
        case CRT_ISEPIC:            cart = new Isepic(c64); break;
        case CRT_GEO_RAM:           cart = new GeoRAM(c64); break;
        case CRT_REU:               cart = new Reu(c64); break;

        default:
            throw Error(ERROR_CRT_UNSUPPORTED, CRTFile::cartridgeTypeName(type));
    }

    cart->init();
    return cart;
}

Cartridge *
Cartridge::makeWithCRTFile(C64 &c64, const CRTFile &file)
{
    auto type = file.cartridgeType();

    // Only proceed if the cartridge ID is valid
    if (!isKnownType(type)) throw Error(ERROR_CRT_UNKNOWN, std::to_string(type));

    // Try to create the cartridge
    Cartridge *cart = makeWithType(c64, file.cartridgeType());

    // Remember powerup values for game line and exrom line
    cart->gameLineInCrtFile = file.initialGameLine();
    cart->exromLineInCrtFile = file.initialExromLine();

    // Load chip packets
    cart->numPackets = 0;
    for (isize i = 0; i < file.chipCount(); i++) {
        cart->loadChip(i, file);
    }

    if (CRT_DEBUG) cart->dump(Category::State);
    return cart;
}

Cartridge::Cartridge(C64 &ref) : SubComponent(ref)
{
    trace(CRT_DEBUG, "Creating cartridge at address %p...\n", (void *)this);
}

Cartridge::~Cartridge()
{
    trace(CRT_DEBUG, "Releasing cartridge...\n");
    dealloc();
}

void
Cartridge::dealloc()
{
    for (isize i = 0; i < numPackets; i++) {
        assert(packet[i] != nullptr);
        delete packet[i];
        packet[i] = nullptr;
    }

    if (externalRam) {
        assert(ramCapacity > 0);
        delete [] externalRam;
        externalRam = nullptr;
    }

    numPackets = 0;
}

void
Cartridge::init()
{
    auto &traits = getCartridgeTraits();
    trace(CRT_DEBUG, "Initializing cartridge %s...\n", traits.title);

    // Allocate external memory (if any)
    setRamCapacity(traits.memory);
}

void
Cartridge::resetCartConfig() {

    expansionPort.setGameAndExrom(gameLineInCrtFile, exromLineInCrtFile);
}

void
Cartridge::cloneRomAndRam(const Cartridge& other)
{
    if (numPackets != other.numPackets) {

        dealloc();

        for (isize i = 0; i < other.numPackets; i++) {

            assert(other.packet[i] != nullptr);
            packet[i] = new CartridgeRom(c64);
            packet[i] = other.packet[i];
        }
    }

    if (writes != other.writes && other.externalRam) {

        if (!externalRam) externalRam = new u8[other.ramCapacity];
        memcpy(externalRam, other.externalRam, ramCapacity);
    }
}

void 
Cartridge::cacheInfo(CartridgeInfo &result) const
{
    result.type = getCartridgeType();
    result.supported = isSupported();
    result.gameLineInCrtFile = gameLineInCrtFile;
    result.exromLineInCrtFile = exromLineInCrtFile;
    result.numPackets = numPackets;
    result.switchPos = switchPos;
    result.led = led;
}

CartridgeRomInfo
Cartridge::getRomInfo(isize nr) const
{
    CartridgeRomInfo result = { };

    if (nr >= 0 && nr < numPackets) {

        result.size = packet[nr]->size;
        result.loadAddress = packet[nr]->loadAddress;

    } else {

        warn("Packet %ld does not exist\n", nr);
    }

    return result;
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

    return packet[chipL] ? packet[chipL]->peek(addr + offsetL) : 0;
}

u8
Cartridge::peekRomH(u16 addr)
{
    assert(addr <= 0x1FFF);
    assert(chipH >= 0 && chipH < numPackets);

    return packet[chipH] ? packet[chipH]->peek(addr + offsetH) : 0;
}

u8
Cartridge::spypeek(u16 addr) const
{
    assert(isROMLaddr(addr) || isROMHaddr(addr));

    u16 relAddr = addr & 0x1FFF;

    // Question: Is it correct to return a value from RAM if no ROM is mapped?
    if (isROMLaddr(addr)) {
        return (relAddr < mappedBytesL) ? spypeekRomL(relAddr) : mem.ram[addr];
    } else {
        return (relAddr < mappedBytesH) ? spypeekRomH(relAddr) : mem.ram[addr];
    }
}

u8
Cartridge::spypeekRomL(u16 addr) const
{
    assert(addr <= 0x1FFF);
    assert(chipL >= 0 && chipL < numPackets);

    return packet[chipL] ? packet[chipL]->spypeek(addr + offsetL) : 0;
}

u8
Cartridge::spypeekRomH(u16 addr) const
{
    assert(addr <= 0x1FFF);
    assert(chipH >= 0 && chipH < numPackets);

    return packet[chipH] ? packet[chipH]->spypeek(addr + offsetH) : 0;
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
    if (!c64.getUltimax()) mem.ram[addr] = value;
}

void
Cartridge::setRamCapacity(isize size)
{
    // Free
    if (externalRam) {

        delete [] externalRam;
        ramCapacity = 0;
        externalRam = nullptr;
    }

    // Allocate
    if (size > 0) {

        externalRam = new u8[size];
        ramCapacity = (u64)size;
        memset(externalRam, 0xFF, size);
    }
}

u8
Cartridge::peekRAM(u32 addr) const
{
    assert(addr < ramCapacity);
    return externalRam[addr];
}

void
Cartridge::pokeRAM(u32 addr, u8 value)
{
    assert(addr < ramCapacity);
    externalRam[addr] = value;
    writes++;
}

void
Cartridge::eraseRAM(u8 value)
{
    assert(externalRam != nullptr);
    memset(externalRam, value, ramCapacity);
    writes += ramCapacity;
}

void
Cartridge::loadChip(isize nr, const CRTFile &crt)
{
    assert(nr < MAX_PACKETS);

    u16 size = crt.chipSize(nr);
    u16 start = crt.chipAddr(nr);
    u16 type = crt.chipType(nr);

    // Perform some consistency checks
    if (start < 0x8000) {
        warn("Ignoring chip %ld: Start address too low (%04X)\n", nr, start);
        return;
    }
    if (0x10000 - start < size) {
        warn("Ignoring chip %ld: Invalid size (start: %04X size: %04X)/n", nr, start, size);
        return;
    }

    // Delete old chip packet if present
    if (packet[nr]) {
        delete packet[nr];
    }

    // Create new chip packet
    switch (type) {

        case 0: // ROM
            packet[nr] = new CartridgeRom(c64, size, start, crt.chipData(nr));
            break;

        case 1: // RAM
            warn("Ignoring chip %ld, because it has type RAM.\n", nr);
            return;

        case 2: // Flash ROM
            warn("Chip %ld is a Flash Rom. Creating a Rom instead.\n", nr);
            packet[nr] = new CartridgeRom(c64, size, start, crt.chipData(nr));
            break;

        default:
            warn("Ignoring chip %ld, because it has unknown type %d.\n", nr, type);
            return;
    }

    numPackets++;
}

void
Cartridge::bankInROML(isize nr, u16 size, u16 offset)
{
    chipL = nr;
    mappedBytesL = size;
    offsetL = offset;
}

void
Cartridge::bankInROMH(isize nr, u16 size, u16 offset)
{
    chipH = nr;
    mappedBytesH = size;
    offsetH = offset;
}

void
Cartridge::bankIn(isize nr)
{
    assert(nr < MAX_PACKETS);

    if (packet[nr] == nullptr)
        return;

    assert(packet[nr]->size <= 0x4000);

    if (packet[nr]->mapsToLH()) {

        bankInROML(nr, 0x2000, 0); // chip covers ROML and (part of) ROMH
        bankInROMH(nr, packet[nr]->size - 0x2000, 0x2000);
        debug(CRT_DEBUG, "Banked in chip %ld in ROML and ROMH\n", nr);

    } else if (packet[nr]->mapsToL()) {

        bankInROML(nr, packet[nr]->size, 0); // chip covers (part of) ROML
        debug(CRT_DEBUG, "Banked in chip %ld in ROML\n", nr);

    } else if (packet[nr]->mapsToH()) {

        bankInROMH(nr, packet[nr]->size, 0); // chip covers (part of) ROMH
        debug(CRT_DEBUG, "Banked in chip %ld to ROMH\n", nr);

    } else {

        warn("Cannot map chip %ld. Invalid start address.\n", nr);
    }
}

void
Cartridge::bankOut(isize nr)
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
Cartridge::setSwitch(isize pos)
{
    switchPos = pos;
}

}
