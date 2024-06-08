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
#include "Option.h"

namespace vc64 {

//
// Methods from Dumpable
//

void
Emulator::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::Debug) {

        for (isize i = DebugFlagEnum::minVal; i < DebugFlagEnum::maxVal; i++) {

            os << tab(DebugFlagEnum::key(i));
            os << bol(main.getDebugVariable(DebugFlag(i))) << std::endl;
        }
    }

    if (category == Category::Defaults) {

        defaults.dump(category, os);
    }

    if (category == Category::RunAhead) {

        os << "Primary instance:" << std::endl << std::endl;

        os << tab("Frame");
        os << dec(main.frame) << std::endl;
        os << tab("Beam");
        os << "(" << dec(main.scanline) << "," << dec(main.rasterCycle) << ")" << std::endl;
        os << tab("Cycle");
        os << dec(main.cpu.clock) << std::endl << std::endl;

        os << "Run-ahead instance:" << std::endl << std::endl;

        os << tab("Clone nr");
        os << dec(clones) << std::endl;
        os << tab("Frame");
        os << dec(ahead.frame) << std::endl;
        os << tab("Beam");
        os << " (" << dec(ahead.scanline) << "," << dec(ahead.rasterCycle) << ")" << std::endl;
        os << tab("Cycle");
        os << dec(ahead.cpu.clock) << std::endl;
    }

    if (category == Category::State) {

        os << tab("Execution state");
        os << ExecStateEnum::key(state) << std::endl;
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
    }
}


//
// Methods from Inspectable
//

void
Emulator::cacheInfo(EmulatorInfo &result) const
{
    {   SYNCHRONIZED
        
        result.state = state;
        result.refreshRate = isize(refreshRate());
        result.powered = isPoweredOn();
        result.paused = isPaused();
        result.running = isRunning();
        result.suspended = isSuspended();
        result.warping = isWarping();
        result.tracking = isTracking();
    }
}

void
Emulator::cacheStats(EmulatorStats &result) const
{
    {   SYNCHRONIZED
        
        result.cpuLoad = cpuLoad;
        result.fps = fps;
        result.resyncs = resyncs;
    }

}


//
// Methods from Configurable
//

void
Emulator::resetConfig()
{
    Configurable::resetConfig(defaults);
}

i64
Emulator::getOption(Option opt) const
{
    switch (opt) {

        case OPT_EMU_WARP_BOOT:         return config.warpBoot;
        case OPT_EMU_WARP_MODE:         return config.warpMode;
        case OPT_EMU_VSYNC:             return config.vsync;
        case OPT_EMU_SPEED_ADJUST:      return config.speedAdjust;
        case OPT_EMU_SNAPSHOTS:         return config.snapshots;
        case OPT_EMU_SNAPSHOT_DELAY:    return config.snapshotDelay;
        case OPT_EMU_RUN_AHEAD:         return config.runAhead;

        default:
            fatalError;
    }
}

void
Emulator::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_EMU_WARP_BOOT:

            return;

        case OPT_EMU_WARP_MODE:

            if (!WarpModeEnum::isValid(value)) {
                throw Error(ERROR_OPT_INV_ARG, WarpModeEnum::keyList());
            }
            return;

        case OPT_EMU_VSYNC:

            return;

        case OPT_EMU_SPEED_ADJUST:

            if (value < 50 || value > 200) {
                throw Error(ERROR_OPT_INV_ARG, "50...200");
            }
            return;

        case OPT_EMU_SNAPSHOTS:

            return;

        case OPT_EMU_SNAPSHOT_DELAY:

            if (value < 10 || value > 3600) {
                throw Error(ERROR_OPT_INV_ARG, "10...3600");
            }
            return;

        case OPT_EMU_RUN_AHEAD:

            if (value < 0 || value > 12) {
                throw Error(ERROR_OPT_INV_ARG, "0...12");
            }
            return;

        default:
            throw Error(ERROR_OPT_UNSUPPORTED);
    }
}

void
Emulator::setOption(Option opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case OPT_EMU_WARP_BOOT:

            config.warpBoot = isize(value);
            return;

        case OPT_EMU_WARP_MODE:

            config.warpMode = WarpMode(value);
            return;

        case OPT_EMU_VSYNC:

            config.vsync = bool(value);
            return;

        case OPT_EMU_SPEED_ADJUST:

            config.speedAdjust = isize(value);
            main.updateClockFrequency();
            return;

        case OPT_EMU_SNAPSHOTS:

            config.snapshots = bool(value);
            main.scheduleNextSNPEvent();
            return;

        case OPT_EMU_SNAPSHOT_DELAY:

            config.snapshotDelay = isize(value);
            main.scheduleNextSNPEvent();
            return;

        case OPT_EMU_RUN_AHEAD:

            config.runAhead = isize(value);
            return;

        default:
            fatalError;
    }
}

}
