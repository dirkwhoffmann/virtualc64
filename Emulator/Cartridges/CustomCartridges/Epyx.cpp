// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

void
EpyxFastLoad::_reset()
{
    Cartridge::_reset();
    dischargeCapacitor();
}

size_t
EpyxFastLoad::stateSize()
{
    return Cartridge::stateSize() + 8;
}
void
EpyxFastLoad::didLoadFromBuffer(u8 **buffer)
{
    Cartridge::didLoadFromBuffer(buffer);
    cycle = read64(buffer);
}

void
EpyxFastLoad::didSaveToBuffer(u8 **buffer)
{
    Cartridge::didSaveToBuffer(buffer);
    write64(buffer, cycle);
}

void
EpyxFastLoad::resetCartConfig()
{
    expansionport.setCartridgeMode(CRT_8K);
}

u8
EpyxFastLoad::peekRomL(u16 addr)
{
    dischargeCapacitor();
    return Cartridge::peekRomL(addr);
}

u8
EpyxFastLoad::peekIO1(u16 addr)
{
    dischargeCapacitor();
    return 0;
}

u8
EpyxFastLoad::peekIO2(u16 addr)
{
    // I/O 2 mirrors the last 256 ROM bytes
    return packet[0]->peek(addr & 0x1FFF);
}

void
EpyxFastLoad::execute()
{
    // Switch cartridge off when the capacitor is fully charged
    if (cpu.cycle > cycle) {
        expansionport.setCartridgeMode(CRT_OFF);
    }
}

void
EpyxFastLoad::dischargeCapacitor()
{
    // Switch on cartridge
    expansionport.setCartridgeMode(CRT_8K);
    
    // Schedule cartridge to be switched off in about 512 CPU cycles
    cycle = cpu.cycle + 512;
}
