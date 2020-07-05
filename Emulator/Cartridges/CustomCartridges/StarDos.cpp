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
StarDos::oldReset()
{
    Cartridge::oldReset();
    voltage = 0;
    latestVoltageUpdate = 0;
}

void
StarDos::updateVoltage()
{
    // If the capacitor is untouched, it slowly raises to 2.0V
    
    if (voltage < 2000000 /* 2.0V */) {
        u64 elapsedCycles = cpu.cycle - latestVoltageUpdate;
        voltage += MIN(2000000 - voltage, elapsedCycles * 2);
    }
    latestVoltageUpdate = cpu.cycle;
}

void
StarDos::charge()
{
    updateVoltage();
    voltage += MIN(5000000 /* 5.0V */ - voltage, 78125);
    if (voltage > 2700000 /* 2.7V */) {
        enableROML();
    }
}

void
StarDos::discharge()
{
    updateVoltage();
    voltage -= MIN(voltage, 78125);
    if (voltage < 1400000 /* 1.4V */) {
        disableROML();
    }
}

void
StarDos::enableROML()
{
    expansionport.setExromLine(0);
}

void
StarDos::disableROML()
{
    expansionport.setExromLine(1);
}

void
StarDos::updatePeekPokeLookupTables()
{
    // Replace Kernel by the StarDos kernel
    if (mem.peekSrc[0xE] == M_KERNAL) {
        mem.peekSrc[0xE] = M_CRTHI;
        mem.peekSrc[0xF] = M_CRTHI;
    }
}
