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
public Inspectable<EmulatorInfo, EmulatorStats>, public Configurable {

    friend class API;
    friend class VirtualC64;

    ConfigOptions options = {

        OPT_EMU_WARP_BOOT,
        OPT_EMU_WARP_MODE,
        OPT_EMU_VSYNC,
        OPT_EMU_SPEED_ADJUST,
        OPT_EMU_SNAPSHOTS,
        OPT_EMU_SNAPSHOT_DELAY,
        OPT_EMU_RUN_AHEAD
    };
    
    EmulatorConfig config = { };

    // The virtual C64
    C64 main = C64(*this, 0);

    // The run-ahead instance
    C64 ahead = C64(*this, 1);

    // Keeps track of the number of recreated run-ahead instances
    isize clones = 0;

public:

    // Storage for external events
    CmdQueue cmdQueue;

    // User default settings
    static Defaults defaults;

    // Information about the host system
    Host host = Host(*this);


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

    // Checks the initialization state
    bool isInitialized() const;

    // Marks the run-ahead instance as dirty
    [[deprecated]] void markAsDirty() { main.markAsDirty(); }


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
    // Methods from Configurable
    //

public:

    const ConfigOptions &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;


    //
    // Main entry points for configuring the emulator
    //

public:

    // Queries an option
    i64 get(Option opt, isize id = 0) const;

    // Checks an option
    void check(Option opt, i64 value, std::optional<isize> id = std::nullopt);

    // Sets an option
    void set(Option opt, i64 value, std::optional<isize> id = std::nullopt);

    // Convenience wrappers
    void set(Option opt, const string &value) throws;
    void set(Option opt, const string &value, isize id) throws;
    void set(const string &opt, const string &value) throws;
    void set(const string &opt, const string &value, isize id) throws;

    // Configures the emulator to match a specific C64 model
    void set(C64Model model);

private:

    const EmulatorConfig &getConfig() const { return config; }
    void resetConfig();

    // Returns the target component for an option
    std::vector<Configurable *> routeOption(Option opt);
    std::vector<const Configurable *> routeOption(Option opt) const;

    // Overrides a config option if the corresponding debug option is enabled
    i64 overrideOption(Option opt, i64 value) const;

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
    void recreateRunAheadInstance();

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

    double refreshRate() const override;


    //
    // Execution control
    //

public:

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

private:

    // Processes a command from the command queue
    void process(const Cmd &cmd);
};

}
