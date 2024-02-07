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

        OPT_WARP_MODE,
    };

    for (auto &option : options) {
        setConfigItem(option, defaults.get(option));
    }
}

i64
Emulator::getConfigItem(Option option) const
{
    switch (option) {

        case OPT_WARP_MODE:     return config.warpMode;

        default:
            fatalError;
    }
}

void
Emulator::setConfigItem(Option option, i64 value)
{
    switch (option) {

        case OPT_WARP_MODE:

            if (!WarpModeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, WarpModeEnum::keyList());
            }

            config.warpMode = WarpMode(value);
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

    if (c64.cpu.clock < SEC(c64.config.warpBoot)) {

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
    return c64.getSyncMode();
}

void 
Emulator::execute()
{
    c64.execute();
}

double 
Emulator::refreshRate() const
{
    return c64.refreshRate();
}

util::Time 
Emulator::wakeupPeriod() const
{
    return c64.wakeupPeriod();
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
