// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "PowerSupply.h"
#include "C64.h"

PowerSupply::PowerSupply(C64& ref) : SubComponent(ref)
{

}

void
PowerSupply::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
}

PowerSupplyConfig
PowerSupply::getDefaultConfig()
{
    PowerSupplyConfig defaults;

    defaults.powerGrid = GRID_STABLE_50HZ;
    
    return defaults;
}

void
PowerSupply::resetConfig()
{
    PowerSupplyConfig defaults = getDefaultConfig();
    
    setConfigItem(OPT_POWER_GRID, defaults.powerGrid);
}

i64
PowerSupply::getConfigItem(Option option) const
{
    switch (option) {
            
        case OPT_POWER_GRID:  return config.powerGrid;
        
        default:
            assert(false);
            return 0;
    }
}

void
PowerSupply::setConfigItem(Option option, i64 value)
{
    switch (option) {
            
        case OPT_POWER_GRID:
            
            if (!PowerGridEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INV_ARG, PowerGridEnum::keyList());
            }
            
            config.powerGrid = (PowerGrid)value;
            return;

        default:
            return;
    }
}

Cycle
PowerSupply::todTickDelay(u8 cra)
{
    Cycle delay, jitter;
    i64 frequency = vic.isPAL() ? PAL_CLOCK_FREQUENCY : NTSC_CLOCK_FREQUENCY;
    
    switch (config.powerGrid) {

        case GRID_STABLE_50HZ:
            
            delay = (cra & 0x80) ? frequency / 10 : frequency * 6/50;
            jitter = 0;
            break;
            
        case GRID_UNSTABLE_50HZ:
            
            delay = (cra & 0x80) ? frequency / 10 : frequency * 6/50;
            jitter = (rand() % 1000) - 500;
            break;

        case GRID_STABLE_60HZ:
            
            delay = (cra & 0x80) ? frequency * 5/60 : frequency / 10;
            jitter = 0;
            break;
            
        case GRID_UNSTABLE_60HZ:
            
            delay = (cra & 0x80) ? frequency * 5/60 : frequency / 10;
            jitter = (rand() % 1000) - 500;
            break;

        default:
            assert(false);
            return 0;
    }

    return delay + jitter;
}
