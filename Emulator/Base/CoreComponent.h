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
#include "Configurable.h"
#include "Inspectable.h"
#include "Concurrency.h"
#include "Suspendable.h"
#include "ThreadTypes.h"
#include <vector>
#include <functional>

namespace vc64 {

struct Description {

    const char *name;
    const char *description;
};

typedef std::vector<Description> Descriptions;

class CoreComponent :
public CoreObject, public Serializable, public Suspendable, public Synchronizable, public Configurable {

public:

    // Reference to the emulator this instance belongs to
    class Emulator &emulator;

    // Object identifier (to distinguish instances of the same component)
    const isize objid;

    // Subcomponents
    std::vector<CoreComponent *> subComponents;


    //
    // Initializing
    //
    
public:

    CoreComponent(Emulator& ref, isize id = 0) : emulator(ref), objid(id) { }

    virtual const Descriptions &getDescriptions() const = 0;
    const char *objectName() const override;
    const char *description() const override;

    bool operator== (CoreComponent &other);
    bool operator!= (CoreComponent &other) { return !(other == *this); }

    /* This function is called inside the emulator's launch routine. It iterates
     * through all components and calls the _initialize() delegate.
     */
    void initialize();
    virtual void _initialize() { }

    // Main reset routines
    void hardReset();
    void softReset();

    /* This function is called inside the C64 reset routines. It iterates
     * through all components and calls the _reset() delegate.
     */
    void reset(bool hard);
    virtual void _reset(bool hard) { }

    // Returns the fallback value for a config option
    i64 getFallback(Option opt) const override;

    // Resets the configuration of this component and all subcomponents
    virtual void resetConfig();

    //
    void routeOption(Option opt, std::vector<Configurable *> &result);


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

    void suspend() override;
    void resume() override;

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
    void focus();
    void unfocus();

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
    virtual void _focus() { }
    virtual void _unfocus() { }


    //
    // Misc
    //

public:
    
    bool isEmulatorThread() const;

    // Experimental
    void exportConfig(std::ostream& ss, bool diff = false) const;
    void exportDiff(std::ostream& ss) const { exportConfig(ss, true); }
};

}
