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

#include "C64.h"
#include "Defaults.h"
#include "EmulatorTypes.h"
#include "Host.h"
#include "Thread.h"

namespace vc64 {

class Emulator : public Thread {

public:

    // User default settings
    static Defaults defaults;

    // Information about the host system
    Host host = Host(*this);

    // The virtual C64 TODO: MAKE THIS PRIVATE EVENTUALLY
    C64 _c64 = C64(*this);

private:

    // The current configuration
    EmulatorConfig config = {};


    //
    // Initializing
    //

public:

    Emulator();
    ~Emulator();

private:

    // Initializes all components
    void initialize();


    //
    // Configuring
    //

private:

    static EmulatorConfig getDefaultConfig();
    const EmulatorConfig &getConfig() const { return config; }
    void resetConfig();

    // Overrides a config option if the corresponding debug option is enabled
    i64 overrideOption(Option option, i64 value);

    // Powers off and resets the emulator to it's initial state
    void revertToFactorySettings();


    //
    // Methods from CoreObject
    //

private:

    const char *getDescription() const override { return "Emulator"; }
    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from Thread
    //

private:

    void stateChange(ThreadTransition transition) override;
    void isReady() override;
    bool shouldWarp() override;
    isize missingFrames() const override;
    void computeFrame() override;


    //
    // Public API
    //

private:
    
    struct API : Suspendable, References {

        class Emulator &emulator;

        API(Emulator& ref) : References(ref._c64), emulator(ref) { }

        void suspend() { emulator.suspend(); }
        void resume() { emulator.resume(); }

        bool isUserThread() const { return !emulator.isEmulatorThread(); }
    };

public:

    #include "API.h"

};

}
