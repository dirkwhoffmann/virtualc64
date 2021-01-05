// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"

#ifdef __MACH__
#include <mach/mach_time.h>
#endif

class Oscillator : public C64Component {
    
#ifdef __MACH__

    // Information about the Mach system timer
    static mach_timebase_info_data_t tb;

    // Converts kernel time to nanoseconds
    static u64 abs_to_nanos(u64 abs) { return abs * tb.numer / tb.denom; }
    
    // Converts nanoseconds to kernel time
    static u64 nanos_to_abs(u64 nanos) { return nanos * tb.denom / tb.numer; }

#endif
    
    /* The heart of this class is method sychronize() which puts the thread to
     * sleep for a certain interval. In order to calculate the delay, the
     * function needs to know the values of the C64 clock and the Kernel
     * clock at the time the synchronization timer was started. The values are
     * stores in the following two variables and recorded in restart().
     */
    
    // C64 clock
    Cycle clockBase = 0;

    // Kernel clock (Nanoseconds)
    u64 timeBase = 0;

    
    //
    // Constructing
    //
    
public:
    
    Oscillator(C64& ref);
    const char *getDescription() override;

private:
    
    void _reset() override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }

    template <class T>
    void applyToHardResetItems(T& worker)
    {
        worker & clockBase;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
    }

    usize _size() override { COMPUTE_SNAPSHOT_SIZE }
    usize _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    usize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Reading the system clock
    //
    
public:

    // Returns the current kernel time the nano seconds
    static u64 nanos();
    
    
    //
    // Managing emulation speed
    //
        
    // Restarts the synchronization timer
    void restart();

    // Puts the emulator thread to rest
    void synchronize();
    
private:
    
    // Puts the thread to rest until the target time has been reached
    void waitUntil(u64 deadline);
};
