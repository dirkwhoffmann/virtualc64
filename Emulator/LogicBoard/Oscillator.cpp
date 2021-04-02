// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
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
Oscillator::_reset()
{
    RESET_SNAPSHOT_ITEMS
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
    
    // Only proceed if we are not running in warp mode
    if (warpMode) return;
    
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
