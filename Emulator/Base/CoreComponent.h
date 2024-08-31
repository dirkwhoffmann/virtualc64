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

#include "CoreComponentTypes.h"
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

    const CType type;               // Class identifier
    const char *name;               // Short name
    const char *description;        // Textual descripiton
    const char *shell;              // RetroShell access
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
    // Initializers
    //
    
public:

    CoreComponent(Emulator& ref, isize id = 0) : emulator(ref), objid(id) { }


    //
    // Operators
    //

public:

    bool operator== (CoreComponent &other);
    bool operator!= (CoreComponent &other) { return !(other == *this); }


    //
    // Querying properties
    //

public:

    // Returns the description struct of this component
    virtual const Descriptions &getDescriptions() const = 0;

    // Returns certain elements from the description struct
    const char *objectName() const override;
    const char *description() const override;
    const char *shellName() const;

    // State properties (see Thread class for details)
    virtual bool isInitialized() const;
    virtual bool isPoweredOff() const;
    virtual bool isPoweredOn() const;
    virtual bool isPaused() const;
    virtual bool isRunning() const;
    virtual bool isSuspended() const;
    virtual bool isHalted() const;

    // Throws an exception if the emulator is not ready to power on
    virtual void isReady() const throws;

    // Computes a checksum
    u64 checksum(bool recursive);


    //
    // Suspending and Resuming
    //

    // Suspends or resumes the emulator thread
    void suspend() override;
    void resume() override;


    //
    // Configuring
    //

public:

    // Initializes all configuration items with their default values
    virtual void resetConfig();

    // Returns the target component for a given configuration option
    Configurable *routeOption(Option opt, isize objid);

    // Returns the fallback value for a config option
    i64 getFallback(Option opt) const override;


    //
    // Controlling the state
    //

public:

    void initialize();
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


    //
    // Performing state changes
    //

private:

    virtual void _initialize() { }
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
    // Serializing
    //

public:

    // Returns the size of the internal state in bytes
    isize size(bool recursive = true);

    // Resets the internal state
    void reset(bool hard);
    virtual void _willReset(bool hard) { }
    virtual void _didReset(bool hard) { }

    // Convenience wrappers
    void hardReset() { reset(true); }
    void softReset() { reset(false); }

    // Loads the internal state from a memory buffer
    isize load(const u8 *buf) throws;
    virtual void _didLoad() { }

    // Saves the internal state to a memory buffer
    isize save(u8 *buf);
    virtual void _didSave() { }


    //
    // Working with subcomponents
    //

public:

    // Collects references to this components and all subcomponents
    std::vector<CoreComponent *> collectComponents();
    void collectComponents(std::vector<CoreComponent *> &result);

    // Traverses the component tree and applies a function
    void preoderWalk(std::function<void(CoreComponent *)> func);
    void postorderWalk(std::function<void(CoreComponent *)> func);


    //
    // Misc
    //

public:
    
    bool isEmulatorThread() const;

    // Compares two components and reports differences (for debugging)
    void diff(CoreComponent &other);

    // Exports the current configuration to a script file
    void exportConfig(std::ostream& ss, bool diff = false) const;

    // Exports only those options that differ from the default config
    void exportDiff(std::ostream& ss) const { exportConfig(ss, true); }
};

}
