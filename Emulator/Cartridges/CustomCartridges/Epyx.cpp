// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"

void
Epyx::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
    Cartridge::_reset(hard);
    
    dischargeCapacitor();
}

void
Epyx::resetCartConfig()
{
    expansionport.setCartridgeMode(CRTMODE_8K);
}

u8
Epyx::peekRomL(u16 addr)
{
    dischargeCapacitor();
    return Cartridge::peekRomL(addr);
}

u8
Epyx::spypeekRomL(u16 addr) const
{
    return Cartridge::spypeekRomL(addr);
}

u8
Epyx::peekIO1(u16 addr)
{
    dischargeCapacitor();
    return 0;
}

u8
Epyx::spypeekIO1(u16 addr) const
{
    return 0;
}

u8
Epyx::peekIO2(u16 addr)
{
    return const_cast<const Epyx*>(this)->spypeekIO2(addr);
}

u8
Epyx::spypeekIO2(u16 addr) const
{
    // I/O 2 mirrors the last 256 ROM bytes
    return packet[0]->peek(addr & 0x1FFF);
}

void
Epyx::execute()
{
    // Switch cartridge off when the capacitor is fully charged
    if (cpu.clock > cycle) {
        expansionport.setCartridgeMode(CRTMODE_OFF);
    }
}

void
Epyx::dischargeCapacitor()
{
    // Switch on cartridge
    expansionport.setCartridgeMode(CRTMODE_8K);
    
    // Schedule cartridge to be switched off in about 512 CPU cycles
    cycle = cpu.clock + 512;
}
