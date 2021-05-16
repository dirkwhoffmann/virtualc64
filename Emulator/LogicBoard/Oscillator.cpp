// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Oscillator.h"
#include "C64.h"

Oscillator::Oscillator(C64& ref) : C64Component(ref)
{

}

const char *
Oscillator::getDescription() const
{
#ifdef __MACH__
    return "Oscillator (Mac)";
#else
    return "Oscillator (Generic)";
#endif
}

void
Oscillator::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
}

OscillatorConfig
Oscillator::getDefaultConfig()
{
    OscillatorConfig defaults;

    defaults.powerGrid = GRID_STABLE_50HZ;
    
    return defaults;
}

void
Oscillator::resetConfig()
{
    OscillatorConfig defaults = getDefaultConfig();
    
    setConfigItem(OPT_POWER_GRID, defaults.powerGrid);
}

i64
Oscillator::getConfigItem(Option option) const
{
    switch (option) {
            
        case OPT_POWER_GRID:  return config.powerGrid;
        
        default:
            assert(false);
            return 0;
    }
}

bool
Oscillator::setConfigItem(Option option, i64 value)
{
    switch (option) {
            
        case OPT_POWER_GRID:
            
            if (!PowerGridEnum::isValid(value)) {
                throw ConfigArgError(PowerGridEnum::keyList());
            }
            
            config.powerGrid = (PowerGrid)value;
            return true;

        default:
            return false;
    }
}

void
Oscillator::restart()
{
    clockBase = cpu.cycle;
    timeBase = util::Time::now();
}

void
Oscillator::synchronize()
{
    syncCounter++;
        
    auto now          = util::Time::now();
    auto elapsedCyles = cpu.cycle - clockBase;
    auto elapsedNanos = util::Time((i64)(elapsedCyles * 1000 * 1000000 / vic.getFrequency()));
    auto targetTime   = timeBase + elapsedNanos;
    
    // Check if we're running too slow...
    if (now > targetTime) {
        
        // Check if we're completely out of sync...
        if ((now - targetTime).asMilliseconds() > 200) {
            
            // warn("The emulator is way too slow (%f).\n", (now - targetTime).asSeconds());
            restart();
            return;
        }
    }
    
    // Check if we're running too fast...
    if (now < targetTime) {
        
        // Check if we're completely out of sync...
        if ((targetTime - now).asMilliseconds() > 200) {
            
            warn("The emulator is way too fast (%f).\n", (targetTime - now).asSeconds());
            restart();
            return;
        }
        
        // See you soon...
        loadClock.stop();
        targetTime.sleepUntil();
        loadClock.go();
    }
    
    // Compute the CPU load once in a while
    if (syncCounter % 32 == 0) {
        
        auto used  = loadClock.getElapsedTime().asSeconds();
        auto total = nonstopClock.getElapsedTime().asSeconds();
        
        cpuLoad = used / total;
        
        loadClock.restart();
        nonstopClock.restart();
    }
}

Cycle
Oscillator::todTickDelay(u8 cra)
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
