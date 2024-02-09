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

#include "Thread.h"
#include "EmulatorTypes.h"
#include "C64.h"

namespace vc64 {

class Emulator : public Thread {

public:

    // The virtual C64
    C64 c64 = C64(*this);

private:

    // The current configuration
    EmulatorConfig config = {};


    //
    // Initializing
    //

public:

    Emulator();
    ~Emulator();

    // Initializes all components
    void initialize();

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

    // Powers off and resets the emulator to it's initial state
    void revertToFactorySettings();

public:

    static EmulatorConfig getDefaultConfig();
    const EmulatorConfig &getConfig() const { return config; }
    void resetConfig();

    i64 getConfigItem(Option option) const;
    void setConfigItem(Option option, i64 value);

private:

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

public:

    double refreshRate() const override;
    void stateChange(ThreadTransition transition) override;

private:

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
};

}
