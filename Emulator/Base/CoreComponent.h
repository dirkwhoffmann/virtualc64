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

#include "EmulatorTypes.hpp"
#include "CoreObject.h"
#include "Synchronizable.h"
#include "Serializable.h"
#include "Configurable.h"
#include "Concurrency.hpp"
#include "Suspendable.h"
#include "ThreadTypes.h"
#include <vector>

namespace vc64 {

struct Description {

    const char *name;
    const char *shellName;
    const char *description;
};

typedef std::vector<Description> Descriptions;

class CoreComponent :
public CoreObject, public Serializable, public Suspendable, public Synchronizable, public Configurable {

public:

    // Dummy description
    constexpr static Descriptions descriptions = { };

    // Reference to the emulator this instance belongs to
    class Emulator &emulator;

    // Identifier (to distinguish floppy drives etc.)
    const isize id;

    // Sub components
    std::vector<CoreComponent *> subComponents;

    // Set to false to silence all debug messages for this component
    bool verbose = true;


    //
    // Initializing
    //
    
public:

    CoreComponent(Emulator& ref) : emulator(ref), id(0) { }
    CoreComponent(Emulator& ref, isize id) : emulator(ref), id(id) { }

    virtual const Descriptions &getDescriptions() const { return descriptions; }

    const char *objectName() const override;
    const char *shellName() const;
    const char *description() const;

    std::vector<CoreComponent *> collectComponents() const;
    void collectComponents(std::vector<CoreComponent *> &components) const;

    bool operator== (CoreComponent &other);
    bool operator!= (CoreComponent &other) { return !(other == *this); }

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

    // Returns the fallback value for a config option
    i64 getFallback(Option opt) const override;

    // Resets the current configuration
    virtual void resetConfig();


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
    // Misc
    //

public:
    
    bool isEmulatorThread() const;

    // Experimental
    void exportConfig(std::ostream& ss, bool diff = false) const;
    void exportDiff(std::ostream& ss) const { exportConfig(ss, true); }
};

/* This class exposes references to all subcomponents of the C64. It's purpose
 * is the simply access to components. E.g., VICII can be accessed via vicii
 * instead of c64.vicii. */
class References {

protected:

    class C64 &c64;
    class CIA1 &cia1;
    class CIA2 &cia2;
    class ControlPort &port1;
    class ControlPort &port2;
    class CPU &cpu;
    class Datasette &datasette;
    class Debugger &debugger;
    class Drive &drive8;
    class Drive &drive9;
    class ExpansionPort &expansionport;
    class Host &host;
    class IEC &iec;
    class Keyboard &keyboard;
    class C64Memory &mem;
    class MsgQueue &msgQueue;
    class ParCable &parCable;
    class PowerSupply &powerSupply;
    class Recorder &recorder;
    class RegressionTester &regressionTester;
    class RetroShell &retroShell;
    class Muxer &muxer;
    class VICII &vic;

    Drive *drive[2] = { &drive8, &drive9 };

public:

    References(C64& ref);
};

/* Base class for all subcomponents of the C64.
 */
class SubComponent : public CoreComponent, public References {

public:

    SubComponent(C64& ref);
    SubComponent(C64& ref, isize id);

    void prefix() const override;
};

}
