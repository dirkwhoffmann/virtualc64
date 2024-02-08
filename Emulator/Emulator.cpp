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

#include "config.h"
#include "Emulator.h"

namespace vc64 {

Emulator::Emulator()
{
    // trace(RUN_DEBUG, "Creating emulator\n");
    resetConfig(); // TODO: DELETE AND CALL SOMEWHERE ELSE
}

Emulator::~Emulator()
{

}

void 
Emulator::initialize()
{
    resetConfig();
    c64.initialize();
}

void
Emulator::launch(const void *listener, Callback *func)
{
    c64.msgQueue.setListener(listener, func);

    // Initialize all components
    initialize();

    // Reset the emulator
    hardReset();

    // Launch the emulator thread
    Thread::launch();
}

EmulatorConfig
Emulator::getDefaultConfig()
{
    EmulatorConfig defaults;

    defaults.warpMode = WARP_AUTO;

    return defaults;
}

void
Emulator::resetConfig()
{
    assert(isPoweredOff());

    auto &defaults = c64.defaults;

    std::vector <Option> options = {

        OPT_WARP_BOOT,
        OPT_WARP_MODE,
        OPT_VSYNC,
        OPT_TIME_LAPSE,
        OPT_RUN_AHEAD,
    };

    for (auto &option : options) {
        setConfigItem(option, defaults.get(option));
    }
}

i64
Emulator::getConfigItem(Option option) const
{
    switch (option) {

        case OPT_WARP_BOOT:     return config.warpBoot;
        case OPT_WARP_MODE:     return config.warpMode;
        case OPT_VSYNC:         return config.vsync;
        case OPT_TIME_LAPSE:    return config.timeLapse;
        case OPT_RUN_AHEAD:     return config.runAhead;

        default:
            fatalError;
    }
}

void
Emulator::setConfigItem(Option option, i64 value)
{
    switch (option) {

        case OPT_WARP_BOOT:

            config.warpBoot = isize(value);
            // updateWarpState();
            return;

        case OPT_WARP_MODE:

            if (!WarpModeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, WarpModeEnum::keyList());
            }

            config.warpMode = WarpMode(value);
            // updateWarpState();
            return;

        case OPT_VSYNC:

            config.vsync = bool(value);
            return;

        case OPT_TIME_LAPSE:

            if (value < 50 || value > 200) {
                throw VC64Error(ERROR_OPT_INVARG, "50...200");
            }

            config.timeLapse = isize(value);
            return;

        case OPT_RUN_AHEAD:

            if (value < 0 || value > 5) {
                throw VC64Error(ERROR_OPT_INVARG, "0...5");
            }

            config.runAhead = isize(value);
            return;

        default:
            fatalError;
    }
}

void
Emulator::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        os << tab("Warp mode");
        os << WarpModeEnum::key(config.warpMode) << std::endl;
        os << tab("Warp boot");
        os << dec(config.warpBoot) << " seconds" << std::endl;
        os << tab("VSYNC");
        os << bol(config.vsync) << std::endl;
        os << tab("Time lapse");
        os << dec(config.timeLapse) << "%" << std::endl;
        os << tab("Run ahead");
        os << dec(config.runAhead) << " frames" << std::endl;
        os << std::endl;
    }

    if (category == Category::State) {

        os << tab("Power");
        os << bol(isPoweredOn()) << std::endl;
        os << tab("Running");
        os << bol(isRunning()) << std::endl;
        os << tab("Suspended");
        os << bol(isSuspended()) << std::endl;
        os << tab("Warping");
        os << bol(isWarping()) << std::endl;
        os << tab("Tracking");
        os << bol(isTracking()) << std::endl;
        os << std::endl;
        os << tab("Refresh rate");
        os << dec(isize(refreshRate())) << " Fps" << std::endl;
        os << tab("Emulator state");
        os << EmulatorStateEnum::key(state) << std::endl;
    }
}

void
Emulator::isReady()
{
    c64.isReady();
}

bool
Emulator::shouldWarp()
{
    if (c64.cpu.clock < SEC(config.warpBoot)) {

        return true;

    } else {

        switch (config.warpMode) {

            case WARP_AUTO:     return c64.iec.isTransferring() || warp;
            case WARP_NEVER:    return warp;
            case WARP_ALWAYS:   return true;

            default:
                fatalError;
        }
    }
}

isize
Emulator::missingFrames() const
{
    // In VSYNC mode, compute exactly one frame per wakeup call
    if (config.vsync) return 1;

    // Compute the elapsed time
    auto elapsed = util::Time::now() - baseTime;

    // Compute which slice should be reached by now
    auto target = elapsed.asNanoseconds() * i64(refreshRate()) / 1000000000;

    // Compute the number of missing slices
    return isize(target - frameCounter);
}

void
Emulator::computeFrame()
{
    c64.execute();
}

double 
Emulator::refreshRate() const
{
    if (config.vsync) {

        return c64.host.getHostRefreshRate();

    } else {

        return c64.vic.getFps() * config.timeLapse / 100.0;
    }
}

void Emulator::stateChange(ThreadTransition transition)
{
    switch (transition) {

        case    TRANSITION_POWER_OFF:   c64.powerOff(); break;
        case    TRANSITION_POWER_ON:    c64.powerOn(); break;
        case    TRANSITION_PAUSE:       c64.pause(); break;
        case    TRANSITION_RUN:         c64.run(); break;
        case    TRANSITION_HALT:        c64.halt(); break;
        case    TRANSITION_WARP_ON:     c64.warpOn(); break;
        case    TRANSITION_WARP_OFF:    c64.warpOff(); break;
        case    TRANSITION_TRACK_ON:    c64.trackOn(); break;
        case    TRANSITION_TRACK_OFF:   c64.trackOff(); break;

        default:
            break;
    }
}

void 
Emulator::hardReset()
{
    assert(!isEmulatorThread());

    {   SUSPENDED

        c64.reset(true);
    }
}

void
Emulator::softReset()
{
    assert(!isEmulatorThread());

    {   SUSPENDED

        c64.reset(false);
    }
}

}
