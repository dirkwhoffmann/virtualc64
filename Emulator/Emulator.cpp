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
    // subComponents = std::vector<CoreComponent *> { &c64 };

    // Initialize the sync timer
    targetTime = util::Time::now();

    // trace(RUN_DEBUG, "Creating emulator\n");
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
        OPT_SYNC_MODE,
        OPT_AUTO_FPS,
        OPT_PROPOSED_FPS,
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
        case OPT_SYNC_MODE:     return config.syncMode;
        case OPT_AUTO_FPS:      return config.autoFps;
        case OPT_PROPOSED_FPS:  return config.proposedFps;

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

        case OPT_SYNC_MODE:

            if (!SyncModeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, SyncModeEnum::keyList());
            }

            config.syncMode = SyncMode(value);
            c64.updateClockFrequency();
            return;

        case OPT_AUTO_FPS:

            config.autoFps = bool(value);
            return;

        case OPT_PROPOSED_FPS:

            if (value < 25 || value > 120) {
                throw VC64Error(ERROR_OPT_INVARG, "25...120");
            }

            config.proposedFps = isize(value);
            c64.updateClockFrequency();
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
        os << tab("Sync mode");
        os << SyncModeEnum::key(config.syncMode) << std::endl;
        os << tab("Auto fps");
        os << bol(config.autoFps) << std::endl;
        os << tab("Proposed fps");
        os << config.proposedFps << " Fps" << std::endl;
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
        os << tab("Sync mode");
        os << SyncModeEnum::key(getSyncMode()) << std::endl;
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

SyncMode
Emulator::getSyncMode() const
{
    return config.syncMode;
}

void
Emulator::execute()
{
    c64.execute();
}

double 
Emulator::refreshRate() const
{
    switch (config.syncMode) {

        case SYNC_PULSED:

            return c64.host.getHostRefreshRate();

        case SYNC_PERIODIC:
        case SYNC_ADAPTIVE:

            return config.autoFps ? c64.vic.getFps() : config.proposedFps;

        default:
            fatalError;
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
