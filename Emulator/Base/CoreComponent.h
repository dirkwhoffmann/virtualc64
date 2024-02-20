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

#pragma once

#include "EmulatorTypes.h"
#include "CoreObject.h"
#include "Synchronizable.h"
#include "Serializable.h"
#include "Concurrency.h"
#include "Suspendable.h"
#include "ThreadTypes.h"
#include <vector>

namespace vc64 {

struct NoCopy
{
    NoCopy() { };
    NoCopy(NoCopy const&) = delete;
};

struct NoAssign
{
    NoAssign() { };
    NoAssign& operator=(NoAssign const&) = delete;
};

class CoreComponent : NoCopy, // NoAssign,
public CoreObject, public Serializable, public Suspendable, public Synchronizable {

public:

    // Reference to the emulator this instance belongs to
    class Emulator &emulator;

protected:

    // Sub components
    std::vector<CoreComponent *> subComponents;

    // Set to false to silence all debug messages for this component
    bool verbose = true;


    //
    // Initializing
    //
    
public:

    CoreComponent(Emulator& ref) : emulator(ref) { }

    /* This function is called inside the emulator's launch routine. It iterates
     * through all components and calls the _initialize() delegate. By default
     * the initial configuration is setup. 
     */
    void initialize();
    virtual void _initialize() { resetConfig(); }

    /* This function is called inside the C64 reset routines. It iterates
     * through all components and calls the _reset() delegate.
     */
    void reset(bool hard);
    virtual void _reset(bool hard) { }

    //
    // Controlling the state (see Thread class for details)
    //

public:

    virtual bool isPoweredOff() const;
    virtual bool isPoweredOn() const;
    virtual bool isPaused() const;
    virtual bool isRunning() const;
    virtual bool isSuspended() const;
    virtual bool isHalted() const;

    void suspend();
    void resume();

    // Throws an exception if the emulator is not ready to power on
    virtual void isReady() const throws;

protected:

    void powerOn();
    void powerOff();
    void run();
    void pause();
    void halt();
    void warpOn();
    void warpOff();
    void trackOn();
    void trackOff();
    
    void powerOnOff(bool value) { value ? powerOn() : powerOff(); }
    void warpOnOff(bool value) { value ? warpOn() : warpOff(); }
    void trackOnOff(bool value) { value ? trackOn() : trackOff(); }

private:

    virtual void _isReady() const throws { }
    virtual void _powerOn() { }
    virtual void _powerOff() { }
    virtual void _run() { }
    virtual void _pause() { }
    virtual void _halt() { }
    virtual void _warpOn() { }
    virtual void _warpOff() { }
    virtual void _trackOn() { }
    virtual void _trackOff() { }


    //
    // Configuring
    //

public:

    // Initializes all configuration items with their default values
    virtual void resetConfig() { };


    //
    // Misc
    //

    bool isEmulatorThread() const;
};

}
