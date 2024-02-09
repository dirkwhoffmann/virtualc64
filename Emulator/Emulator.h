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

    // The virtual C64
    C64 c64 = C64(*this);

    // Information about the host system
    Host host = Host(*this);

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

public:

    // Launches the emulator thread
    void launch(const void *listener, Callback *func);


    //
    // Configuring
    //

public:

    // Configures a single item
    void configure(Option option, i64 value) throws;
    void configure(Option option, long id, i64 value) throws;

    // Configures the C64 to match a specific C64 model
    void configure(C64Model model);

    i64 getConfigItem(Option option) const;
    i64 getConfigItem(Option option, long id) const;
    void setConfigItem(Option option, i64 value);

private:

    // Powers off and resets the emulator to it's initial state
    void revertToFactorySettings();

    static EmulatorConfig getDefaultConfig();
    const EmulatorConfig &getConfig() const { return config; }
    void resetConfig();

    // Overrides a config option if the corresponding debug option is enabled
    i64 overrideOption(Option option, i64 value);

    
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
    // Running the emulator
    //

public:
    
    void hardReset();
    void softReset();


    //
    // Querying properties
    //

public:

    double refreshRate() const override;
    

};

}
