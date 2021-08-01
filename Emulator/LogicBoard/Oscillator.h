// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "OscillatorTypes.h"
#include "C64Component.h"
#include "Chrono.h"

class Oscillator : public C64Component {
    
    // Current configuration
    OscillatorConfig config = getDefaultConfig();
    
    /* The heart of this class is method sychronize() which puts the thread to
     * sleep for a certain interval. In order to calculate the delay, the
     * function needs to know the values of the C64 clock and the Kernel
     * clock at the time the synchronization timer was started. The values are
     * stores in the following two variables and recorded in restart().
     */
    
    // C64 clock
    Cycle clockBase = 0;

    // Counts the number of calls to 'synchronize'
    isize syncCounter = 0;
    
    // Kernel clock
    util::Time timeBase;

    // The current CPU load (%)
    float cpuLoad = 0.0;
    
    // Clocks for measuring the CPU load
    util::Clock nonstopClock;
    util::Clock loadClock;

    
    //
    // Constructing
    //
    
public:
    
    Oscillator(C64& ref);
    
    const char *getDescription() const override;

private:
    
    void _reset(bool hard) override;
    
    
    //
    // Configuring
    //
    
public:
    
    static OscillatorConfig getDefaultConfig();
    OscillatorConfig getConfig() const { return config; }
    void resetConfig() override;

    i64 getConfigItem(Option option) const;
    void setConfigItem(Option option, i64 value) override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {

    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }

    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Managing emulation speed
    //
       
public:
    
    // Restarts the synchronization timer
    void restart();

    // Puts the emulator thread to rest
    void synchronize();
    
    // Returns the number of CPU cycles between two TOD increments
    Cycle todTickDelay(u8 cra);
};
