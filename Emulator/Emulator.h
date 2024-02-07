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
#include "C64.h"

namespace vc64 {

typedef struct
{
    isize warpBoot;
    WarpMode warpMode;
    SyncMode syncMode;
    bool autoFps;
    isize proposedFps;
}
EmulatorConfig;

class Emulator : public Thread {

    friend class C64;

public:

    // The virtual C64
    C64 c64 = C64(*this);

private:

    // The current configuration
    EmulatorConfig config = {};

    // Warp override mask (to enforce warp mode from outside)
    u8 warpLock = 0;


    //
    // Initializing
    //

public:

    Emulator();
    ~Emulator();

    // Launches the emulator thread
    void launch();
    void launch(const void *listener, Callback *func);


    //
    // Configuring
    //

public:

    static EmulatorConfig getDefaultConfig();
    const EmulatorConfig &getConfig() const { return config; }
    void resetConfig();

    i64 getConfigItem(Option option) const;
    void setConfigItem(Option option, i64 value);



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

    void readyToGo() override;
    void updateWarp() override;
    SyncMode getSyncMode() const override;
    void execute() override;

public:

    double refreshRate() const override;
    util::Time wakeupPeriod() const override;

    void powerOnDelegate() override;
    void powerOffDelegate() override;
    void runDelegate() override;
    void pauseDelegate() override;
    void haltDelegate() override;
    void trackOnDelegate() override;
    void trackOffDelegate() override;


    //
    // Warp mode
    //

    // Switches warp mode on or off
    void warpOn(isize source = 0);
    void warpOff(isize source = 0);
};

}
