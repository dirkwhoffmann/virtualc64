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
EpyxFastLoad::reset()
{
    Cartridge::reset();
    dischargeCapacitor();
}

size_t
EpyxFastLoad::stateSize()
{
    return Cartridge::stateSize() + 8;
}
void
EpyxFastLoad::didLoadFromBuffer(uint8_t **buffer)
{
    Cartridge::didLoadFromBuffer(buffer);
    cycle = read64(buffer);
}

void
EpyxFastLoad::didSaveToBuffer(uint8_t **buffer)
{
    Cartridge::didSaveToBuffer(buffer);
    write64(buffer, cycle);
}

void
EpyxFastLoad::resetCartConfig()
{
    c64->expansionport.setCartridgeMode(CRT_8K);
}

uint8_t
EpyxFastLoad::peekRomL(u16 addr)
{
    dischargeCapacitor();
    return Cartridge::peekRomL(addr);
}

uint8_t
EpyxFastLoad::peekIO1(u16 addr)
{
    dischargeCapacitor();
    return 0;
}

uint8_t
EpyxFastLoad::peekIO2(u16 addr)
{
    // I/O 2 mirrors the last 256 ROM bytes
    return packet[0]->peek(addr & 0x1FFF);
}

void
EpyxFastLoad::execute()
{
    // Switch cartridge off when the capacitor is fully charged
    if (c64->cpu.cycle > cycle) {
        c64->expansionport.setCartridgeMode(CRT_OFF);
    }
}

void
EpyxFastLoad::dischargeCapacitor()
{
    // Switch on cartridge
    c64->expansionport.setCartridgeMode(CRT_8K);
    
    // Schedule cartridge to be switched off in about 512 CPU cycles
    cycle = c64->cpu.cycle + 512;
}
