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
#include "Defaults.h"
#include "C64.h"
#include "Host.h"
#include "Thread.h"
#include "CmdQueue.h"

namespace vc64 {

class Emulator : public Thread, public utl::Synchronizable, public Inspectable<EmulatorInfo, EmulatorStats> {

    friend struct API;
    friend struct VirtualC64;

public:

    // User default settings
    static Defaults defaults;

private:

    // The main emulator instance
    C64 main = C64(*this, 0);

    // The run-ahead instance
    C64 ahead = C64(*this, 1);

    // Indicates if the run-ahead instance needs to be updated
    bool isDirty = true;

    // Incoming external events
    CmdQueue cmdQueue;

    // Texture lock
    utl::Mutex textureLock;


    //
    // Methods
    //

public:

    Emulator();
    ~Emulator();

    // Launches the emulator thread
    void launch(const void *listener, Callback *func);

    // Initializes all components
    void initialize() override;

    // Forces to recreate the run-ahead instance in the next frame
    void markAsDirty() { isDirty = true; }


    //
    // Methods from CoreComponent
    //

public:

    const char *objectName() const override { return "Emulator"; }

private:

    void _dump(Category category, std::ostream &os) const override;


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
    i64 get(Opt opt, isize objid = 0) const;

    // Checks an option
    void check(Opt opt, i64 value, const std::vector<isize> objids = { });

    // Sets an option
    void set(Opt opt, i64 value, const std::vector<isize> objids = { });

    // Convenience wrappers
    void set(Opt opt, const string &value, const std::vector<isize> objids = { });
    void set(const string &opt, const string &value, const std::vector<isize> objids = { });

    // Configures the emulator to match a specific configuration
    void set(ConfigScheme scheme);

    // Powers off and reverts to the default configuration
    void revertToDefaultConfig();


    //
    // Methods from Thread
    //

private:

    void update() override;
    bool shouldWarp() const;
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
    void stepCycle();
    void finishLine();
    void finishFrame();


    //
    // Audio and Video
    //

    const Texture &getTexture() const;
    const Texture &getDmaTexture() const;

    void lockTexture() { textureLock.lock(); }
    void unlockTexture() { textureLock.unlock(); }


    //
    // Command queue
    //

public:

    // Feeds a command into the command queue
    void put(const Command &cmd);
    void put(Cmd type, i64 payload) { put (Command(type, payload)); }
    void put(Cmd type, AlarmCmd payload) { put(Command(type, payload)); }
    void put(Cmd type, ConfigCmd payload) { put(Command(type, payload)); }
    void put(Cmd type, CoordCmd payload) { put(Command(type, payload)); }
    void put(Cmd type, GamePadCmd payload) { put(Command(type, payload)); }
    void put(Cmd type, KeyCmd payload) { put(Command(type, payload)); }
    void put(Cmd type, TapeCmd payload) { put(Command(type, payload)); }


    //
    // Debugging
    //

public:

    // Gets or sets an internal debug variable (only available in debug builds)
    static int getDebugVariable(DebugFlag flag);
    static void setDebugVariable(DebugFlag flag, bool val);
};

}
