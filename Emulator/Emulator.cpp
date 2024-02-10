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

// Perform some consistency checks
static_assert(sizeof(i8 ) == 1, "i8  size mismatch");
static_assert(sizeof(i16) == 2, "i16 size mismatch");
static_assert(sizeof(i32) == 4, "i32 size mismatch");
static_assert(sizeof(i64) == 8, "i64 size mismatch");
static_assert(sizeof(u8 ) == 1, "u8  size mismatch");
static_assert(sizeof(u16) == 2, "u16 size mismatch");
static_assert(sizeof(u32) == 4, "u32 size mismatch");
static_assert(sizeof(u64) == 8, "u64 size mismatch");

Defaults
Emulator::defaults;

Emulator::Emulator() :

c64(*this),
mem(*this),
cpu(*this),
cia1(*this, _c64.cia1),
cia2(*this, _c64.cia2),
vicii(*this),
muxer(*this),
dmaDebugger(*this),
keyboard(*this),
port1(*this, _c64.port1),
port2(*this, _c64.port2)

{
    // trace(RUN_DEBUG, "Creating emulator\n");
    resetConfig(); // TODO: DELETE (MAKE SURE initialize() IS CALLED)
}

Emulator::~Emulator()
{

}

void
Emulator::initialize()
{
    resetConfig();
    _c64.initialize();
}

void
Emulator::revertToFactorySettings()
{
    // Power off the emulator
    powerOff();

    // Put all components into their initial state
    initialize();
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
Emulator::overrideOption(Option option, i64 value)
{
    static std::map<Option,i64> overrides = OVERRIDES;

    if (overrides.find(option) != overrides.end()) {

        msg("Overriding option: %s = %lld\n", OptionEnum::key(option), value);
        return overrides[option];
    }

    return value;
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

    if (category == Category::Defaults) {

        defaults.dump(category, os);
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
    _c64.isReady();
}

bool
Emulator::shouldWarp()
{
    if (_c64.cpu.clock < SEC(config.warpBoot)) {

        return true;

    } else {

        switch (config.warpMode) {

            case WARP_AUTO:     return _c64.iec.isTransferring();
            case WARP_NEVER:    return false;
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
    _c64.execute();
}

double
Emulator::refreshRate() const
{
    if (config.vsync) {

        return host.getHostRefreshRate();

    } else {

        return _c64.vic.getFps() * config.timeLapse / 100.0;
    }
}

void Emulator::stateChange(ThreadTransition transition)
{
    switch (transition) {

        case    TRANSITION_POWER_OFF:   _c64.powerOff(); break;
        case    TRANSITION_POWER_ON:    _c64.powerOn(); break;
        case    TRANSITION_PAUSE:       _c64.pause(); break;
        case    TRANSITION_RUN:         _c64.run(); break;
        case    TRANSITION_HALT:        _c64.halt(); break;
        case    TRANSITION_WARP_ON:     _c64.warpOn(); break;
        case    TRANSITION_WARP_OFF:    _c64.warpOff(); break;
        case    TRANSITION_TRACK_ON:    _c64.trackOn(); break;
        case    TRANSITION_TRACK_OFF:   _c64.trackOff(); break;

        default:
            break;
    }
}

}
