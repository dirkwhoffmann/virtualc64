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

#include "config.hpp"
#include "PowerSupply.hpp"
#include "Emulator.hpp"

namespace vc64 {

PowerSupply::PowerSupply(C64& ref) : SubComponent(ref)
{

}

i64
PowerSupply::getOption(Option option) const
{
    switch (option) {
            
        case OPT_POWER_GRID:  return config.powerGrid;

        default:
            fatalError;
    }
}

void
PowerSupply::setOption(Option option, i64 value)
{
    switch (option) {
            
        case OPT_POWER_GRID:
            
            if (!PowerGridEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, PowerGridEnum::keyList());
            }
            
            config.powerGrid = (PowerGrid)value;
            return;

        default:
            fatalError;
    }
}

void
PowerSupply::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }
}

Cycle
PowerSupply::todTickDelay(u8 cra)
{
    Cycle delay, jitter;
    i64 frequency = vic.pal() ? PAL::CLOCK_FREQUENCY : NTSC::CLOCK_FREQUENCY;

    switch (config.powerGrid) {

        case GRID_STABLE_50HZ:
            
            delay = (cra & 0x80) ? frequency / 10 : frequency * 6/50;
            jitter = 0;
            break;
            
        case GRID_UNSTABLE_50HZ:
            
            delay = (cra & 0x80) ? frequency / 10 : frequency * 6/50;
            jitter = (c64.random() % 1000) - 500;
            break;

        case GRID_STABLE_60HZ:
            
            delay = (cra & 0x80) ? frequency * 5/60 : frequency / 10;
            jitter = 0;
            break;
            
        case GRID_UNSTABLE_60HZ:
            
            delay = (cra & 0x80) ? frequency * 5/60 : frequency / 10;
            jitter = (c64.random() % 1000) - 500;
            break;

        default:
            fatalError;
    }

    return delay + jitter;
}

}
