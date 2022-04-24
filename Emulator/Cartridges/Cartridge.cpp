// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Cartridge.h"
#include "C64.h"

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
            
        case CRT_ISEPIC:
        case CRT_GEO_RAM:
            return true;
            
        default:
            return false;
    }
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
    switch (type) {
            
        case CRT_NORMAL:           return new Cartridge(c64);
        case CRT_ACTION_REPLAY:    return new ActionReplay(c64);
        case CRT_KCS_POWER:        return new KcsPower(c64);
        case CRT_FINAL_III:        return new FinalIII(c64);
        case CRT_SIMONS_BASIC:     return new SimonsBasic(c64);
        case CRT_OCEAN:            return new Ocean(c64);
        case CRT_EXPERT:           return new Expert(c64);
        case CRT_FUNPLAY:          return new Funplay(c64);
        case CRT_SUPER_GAMES:      return new SuperGames(c64);
        case CRT_ATOMIC_POWER:     return new AtomicPower(c64);
        case CRT_EPYX_FASTLOAD:    return new Epyx(c64);
        case CRT_WESTERMANN:       return new Westermann(c64);
        case CRT_REX:              return new Rex(c64);
        case CRT_WARPSPEED:        return new WarpSpeed(c64);
        case CRT_DINAMIC:          return new Dinamic(c64);
        case CRT_ZAXXON:           return new Zaxxon(c64);
        case CRT_MAGIC_DESK:       return new MagicDesk(c64);
        case CRT_COMAL80:          return new Comal80(c64);
        case CRT_STRUCTURED_BASIC: return new StructuredBasic(c64);
        case CRT_MIKRO_ASSEMBLER:  return new MikroAss(c64);
        case CRT_STARDOS:          return new StarDos(c64);
        case CRT_EASYFLASH:        return new EasyFlash(c64);
        case CRT_ACTION_REPLAY3:   return new ActionReplay3(c64);
        case CRT_GAME_KILLER:      return new GameKiller(c64);
        case CRT_FREEZE_FRAME:     return new FreezeFrame(c64);
        case CRT_MACH5:            return new Mach5(c64);
        case CRT_PAGEFOX:          return new PageFox(c64);
        case CRT_KINGSOFT:         return new Kingsoft(c64);
        case CRT_ISEPIC:           return new Isepic(c64);
        case CRT_GEO_RAM:          return new GeoRAM(c64);
            
        default:
            throw VC64Error(ERROR_CRT_UNSUPPORTED,
                            std::to_string(type) + " (" + CartridgeTypeEnum::key(type) + ")");
    }
}

Cartridge *
Cartridge::makeWithCRTFile(C64 &c64, CRTFile &file)
{
    auto type = file.cartridgeType();

    // Only proceed if the cartridge ID is valid
    if (!isKnownType(type)) throw VC64Error(ERROR_CRT_UNKNOWN, std::to_string(type));
    
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
    
    if constexpr (CRT_DEBUG) cart->dump(Category::State);
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
Cartridge::resetCartConfig() {

    expansionport.setGameAndExrom(gameLineInCrtFile, exromLineInCrtFile);
}

void
Cartridge::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
    
    // Reset external RAM
    if (externalRam && !battery) memset(externalRam, 0xFF, ramCapacity);
 
    // Reset all chip packets
    for (isize i = 0; i < numPackets; i++) packet[i]->_reset(hard);
        
    // Bank in visibile chips (chips with low numbers show up first)
    for (int i = MAX_PACKETS - 1; i >= 0; i--) bankIn(i);
}

void
Cartridge::_dump(Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category == Category::State) {
        
        os << tab("Cartridge type");
        os << getCartridgeType() << std::endl;
        os << tab("Game line in CRT");
        os << bol(gameLineInCrtFile) << std::endl;
        os << tab("Exrom line in CRT");
        os << bol(exromLineInCrtFile) << std::endl;
        os << tab("Number of packets");
        os << dec(numPackets) << std::endl;
        
        for (isize i = 0; i < numPackets; i++) {
            
            os << tab("Packet " + std::to_string(i));
            os << dec(packet[i]->size / 1024) << " KB starting at ";
            os << hex(packet[i]->loadAddress) << std::endl;
        }
    }
}

isize
Cartridge::_size()
{
    util::SerCounter counter;
    applyToPersistentItems(counter);
    applyToResetItems(counter);
 
    // Determine size of all packets
    isize packetSize = 0;
    for (isize i = 0; i < numPackets; i++) {
        assert(packet[i] != nullptr);
        packetSize += packet[i]->_size();
    }
    
    return ramCapacity + packetSize + counter.count;
}

isize
Cartridge::_load(const u8 *buffer)
{
    dealloc();
    
    util::SerReader reader(buffer);
    applyToPersistentItems(reader);
    applyToResetItems(reader);
        
    // Load ROM packets
    for (isize i = 0; i < numPackets; i++) {
        assert(packet[i] == nullptr);
        packet[i] = new CartridgeRom(c64);
        reader.ptr += packet[i]->_load(reader.ptr);
    }

    // Load on-board RAM
    if (ramCapacity) {
        assert(externalRam == nullptr);
        externalRam = new u8[ramCapacity];
        for (isize i = 0; i < ramCapacity; i++) externalRam[i] = util::read8(reader.ptr);
    }

    trace(SNP_DEBUG, "Recreated from %ld bytes\n", reader.ptr - buffer);
    return reader.ptr - buffer;
}

isize
Cartridge::_save(u8 *buffer)
{
    util::SerWriter writer(buffer);
    applyToPersistentItems(writer);
    applyToResetItems(writer);
    
    // Save ROM packets
    for (isize i = 0; i < numPackets; i++) {
        assert(packet[i] != nullptr);
        writer.ptr += packet[i]->_save(writer.ptr);
    }
    
    // Save on-board RAM
    if (ramCapacity) {
        assert(externalRam != nullptr);
        for (isize i = 0; i < ramCapacity; i++) {
            util::write8(writer.ptr, externalRam[i]);
        }
    }
    
    trace(SNP_DEBUG, "Serialized %ld bytes\n", writer.ptr - buffer);
    return writer.ptr - buffer;
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

isize
Cartridge::getRamCapacity() const
{
    if (ramCapacity == 0) {
        assert(externalRam == nullptr);
    } else {
        assert(externalRam != nullptr);
    }
    return ramCapacity;
}

void
Cartridge::setRamCapacity(isize size)
{
    // Free
    if (getRamCapacity() > 0) {
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
Cartridge::peekRAM(u16 addr) const
{
    assert(addr < ramCapacity);
    return externalRam[addr];
}

void
Cartridge::pokeRAM(u16 addr, u8 value)
{
    assert(addr < ramCapacity);
    externalRam[addr] = value;
}

void
Cartridge::eraseRAM(u8 value)
{
    assert(externalRam != nullptr);
    memset(externalRam, value, ramCapacity);
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
        warn("Ignoring chip %zd: Start address too low (%04X)\n", nr, start);
        return;
    }
    if (0x10000 - start < size) {
        warn("Ignoring chip %zd: Invalid size (start: %04X size: %04X)/n", nr, start, size);
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
            warn("Ignoring chip %zd, because it has type RAM.\n", nr);
            return;
            
        case 2: // Flash ROM
            warn("Chip %zd is a Flash Rom. Creating a Rom instead.\n", nr);
            packet[nr] = new CartridgeRom(c64, size, start, crt.chipData(nr));
            break;
            
        default:
            warn("Ignoring chip %zd, because it has unknown type %d.\n", nr, type);
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
        // trace(CRT_DEBUG, "Banked in chip %d in ROML and ROMH\n", nr);
    
    } else if (packet[nr]->mapsToL()) {
        
        bankInROML(nr, packet[nr]->size, 0); // chip covers (part of) ROML
        // trace(CRT_DEBUG, "Banked in chip %d in ROML\n", nr);
        
    } else if (packet[nr]->mapsToH()) {
        
        bankInROMH(nr, packet[nr]->size, 0); // chip covers (part of) ROMH
        // trace(CRT_DEBUG, "Banked in chip %d to ROMH\n", nr);
        
    } else {

        warn("Cannot map chip %zd. Invalid start address.\n", nr);
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
    c64.msgQueue.put(MSG_CART_SWITCH);
}
