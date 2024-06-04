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
#include "C64.h"
#include <algorithm>

namespace vc64 {

void
StarDos::_dump(Category category, std::ostream& os) const
{
    using namespace util;
    
    Cartridge::_dump(category, os);
    
    if (category == Category::State) {
        
        os << std::endl;
        
        os << tab("Voltage");
        os << dec(voltage);
        os << tab("Latest Voltage Update");
        os << dec(latestVoltageUpdate);
    }
}

void
StarDos::updatePeekPokeLookupTables()
{
    // Replace Kernal by the StarDos kernal
    if (mem.peekSrc[0xE] == M_KERNAL) {
        mem.peekSrc[0xE] = M_CRTHI;
        mem.peekSrc[0xF] = M_CRTHI;
    }
}

void
StarDos::updateVoltage()
{
    // If the capacitor is untouched, it slowly raises to 2.0V
    
    if (voltage < 2000000 /* 2.0V */) {
        
        i64 elapsedCycles = cpu.clock - latestVoltageUpdate;
        voltage += std::min(2000000 - voltage, elapsedCycles * 2);
    }
    latestVoltageUpdate = cpu.clock;
}

void
StarDos::charge()
{
    updateVoltage();
    voltage += std::min(5000000ULL /* 5.0V */ - voltage, 78125ULL);
    
    if (voltage > 2700000 /* 2.7V */) {
        enableROML();
    }
}

void
StarDos::discharge()
{
    updateVoltage();
    voltage -= std::min(voltage, 78125LL);
    
    if (voltage < 1400000 /* 1.4V */) {
        disableROML();
    }
}

void
StarDos::enableROML()
{
    expansionPort.setExromLine(0);
}

void
StarDos::disableROML()
{
    expansionPort.setExromLine(1);
}

}
