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
    debug(RUN_DEBUG, "Destroying emulator\n");
    if (thread.joinable()) { halt(); }
}

void
Emulator::launch(const void *listener, Callback *func)
{
    c64.msgQueue.setListener(listener, func);

    launch();
}

void
Emulator::launch()
{
    // Make sure to call this function only once
    assert(!thread.joinable());

    // Start the thread and enter the main function
    thread = std::thread(&Thread::main, this);
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
        os << tab("Thread state");
        os << ExecutionStateEnum::key(state) << std::endl;
    }
}

void
Emulator::readyToGo()
{
    c64.isReady();
}

void
Emulator::updateWarp()
{
    u8 oldwarp = warp;

    if (c64.cpu.clock < SEC(config.warpBoot)) {

        warp = 1;

    } else {

        switch (config.warpMode) {

            case WARP_AUTO:     warp = c64.iec.isTransferring(); break;
            case WARP_NEVER:    warp = 0; break;
            case WARP_ALWAYS:   warp = 1; break;

            default:
                fatalError;
        }
    }

    warp |= warpLock;
    
    if (!warp != !oldwarp) {
        warp ? c64.warpOn() : c64.warpOff();
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
Emulator::execute()
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

util::Time 
Emulator::wakeupPeriod() const
{
    return util::Time(i64(1000000000.0 / c64.host.getHostRefreshRate()));
}

void Emulator::powerOnDelegate() { c64.powerOn(); }
void Emulator::powerOffDelegate() { c64.powerOff(); }
void Emulator::runDelegate() { c64.run(); }
void Emulator::pauseDelegate() { c64.pause(); }
void Emulator::haltDelegate() { c64.halt(); }
void Emulator::trackOnDelegate() { c64.trackOn(); }
void Emulator::trackOffDelegate() { c64.trackOff(); }

void Emulator::stateChange(ExecutionState oldState, ExecutionState newState)
{
    

}

void
Emulator::warpOn(isize source)
{
    SUSPENDED SET_BIT(warpLock, source);
}

void
Emulator::warpOff(isize source)
{
    SUSPENDED CLR_BIT(warpLock, source);
}

}
