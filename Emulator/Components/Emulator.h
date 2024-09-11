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
#include "CmdQueue.h"

namespace vc64 {

class VirtualC64;

class Emulator : public Thread, public Synchronizable,
public Inspectable<EmulatorInfo, EmulatorStats> {

    friend class API;
    friend class VirtualC64;

    // The virtual C64
    C64 main = C64(*this, 0);

    // The run-ahead instance
    C64 ahead = C64(*this, 1);

    // Indicates if the run-ahead instance needs to be updated
    bool isDirty = true;

    // Keeps track of the number of recreated run-ahead instances
    isize clones = 0;

public:

    // User default settings
    static Defaults defaults;

    // Incoming external events
    CmdQueue cmdQueue;


    //
    // Methods
    //

public:

    Emulator();
    ~Emulator();

    // Launches the emulator thread
    void launch(const void *listener, Callback *func);

    // Initializes all components
    void initialize();

    // Forces to recreate the run-ahead instance in the next frame
    void markAsDirty() { isDirty = true; }


    //
    // Methods from CoreComponent
    //

public:

    const char *objectName() const override { return "Emulator"; }

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from Inspectable
    //

public:

    void cacheInfo(EmulatorInfo &result) const override;
    void cacheStats(EmulatorStats &result) const override;


    //
    // Main API for configuring the emulator
    //

public:

    // Queries an option
    i64 get(Option opt, isize objid = 0) const throws;

    // Checks an option
    void check(Option opt, i64 value, const std::vector<isize> objids = { }) throws;

    // Sets an option
    void set(Option opt, i64 value, const std::vector<isize> objids = { }) throws;

    // Convenience wrappers
    void set(Option opt, const string &value, const std::vector<isize> objids = { }) throws;
    void set(const string &opt, const string &value, const std::vector<isize> objids = { }) throws;

    // Configures the emulator to match a specific C64 model
    void set(C64Model model);

    // Powers off and resets the emulator to it's initial state
    void revertToFactorySettings();


    //
    // Methods from Thread
    //

private:

    void update() override;
    bool shouldWarp();
    isize missingFrames() const override;
    void computeFrame() override;

    void _powerOn() override { main.powerOn(); }
    void _powerOff() override { main.powerOff(); }
    void _pause() override { main.pause(); }
    void _run() override { main.run(); }
    void _halt() override { main.halt(); }
    void _warpOn() override { main.warpOn(); }
    void _warpOff() override { main.warpOff(); }
    void _trackOn() override { main.trackOn(); }
    void _trackOff() override { main.trackOff(); }

    void isReady() override;

public:

    //  double refreshRate() const override;


    //
    // Managing the run-ahead instance
    //

private:

    // Clones the run-ahead instance
    void cloneRunAheadInstance();

    // Clones the run-ahead instance and fast forwards it to the proper frame
    void recreateRunAheadInstance();


    //
    // Execution control
    //

public:

    void hardReset();
    void softReset();
    void stepInto();
    void stepOver();


    //
    // Audio and Video
    //

    u32 *getTexture() const;
    u32 *getDmaTexture() const;


    //
    // Command queue
    //

public:

    // Feeds a command into the command queue
    void put(const Cmd &cmd);
    void put(CmdType type, i64 payload) { put (Cmd(type, payload)); }


    //
    // Debugging
    //

public:

    // Gets or sets an internal debug variable (only available in debug builds)
    static int getDebugVariable(DebugFlag flag);
    static void setDebugVariable(DebugFlag flag, bool val);
};

}
