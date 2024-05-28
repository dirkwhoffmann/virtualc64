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
                throw VC64Error(ERROR_OPT_INV_ARG, WarpModeEnum::keyList());
            }
            return;

        case OPT_EMU_VSYNC:

            return;

        case OPT_EMU_SPEED_ADJUST:

            if (value < 50 || value > 200) {
                throw VC64Error(ERROR_OPT_INV_ARG, "50...200");
            }
            return;

        case OPT_EMU_SNAPSHOTS:

            return;

        case OPT_EMU_SNAPSHOT_DELAY:

            if (value < 10 || value > 3600) {
                throw VC64Error(ERROR_OPT_INV_ARG, "10...3600");
            }
            return;

        case OPT_EMU_RUN_AHEAD:

            if (value < 0 || value > 12) {
                throw VC64Error(ERROR_OPT_INV_ARG, "0...12");
            }
            return;

        default:
            throw VC64Error(ERROR_OPT_UNSUPPORTED);
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

i64
Emulator::get(Option opt, isize id) const
{
    auto targets = routeOption(opt);
    assert(isize(targets.size()) > id);
    return targets.at(id)->getOption(opt);
}

void
Emulator::check(Option opt, i64 value, std::optional<isize> id)
{
    if (id) {
        debug(CNF_DEBUG, "check(%s, %lld, %ld)\n", OptionEnum::key(opt), value, *id);
    } else {
        debug(CNF_DEBUG, "check(%s, %lld)\n", OptionEnum::key(opt), value);
    }

    if (!isInitialized()) initialize();

    // Check if this option has been locked for debugging
    value = overrideOption(opt, value);

    auto targets = routeOption(opt);

    if (id) {

        // Check a single component
        assert(isize(targets.size()) > *id);
        targets.at(*id)->checkOption(opt, value);

    } else {

        // Check all components
        for (const auto &target : targets) {
            target->checkOption(opt, value);
        }
    }
}

void
Emulator::set(Option opt, i64 value, std::optional<isize> id)
{
    if (id) {
        debug(CNF_DEBUG, "set(%s, %lld, %ld)\n", OptionEnum::key(opt), value, *id);
    } else {
        debug(CNF_DEBUG, "set(%s, %lld)\n", OptionEnum::key(opt), value);
    }

    if (!isInitialized()) initialize();

    // Check if this option is overridden for debugging
    value = overrideOption(opt, value);

    // Determine the receiver for this option
    auto targets = routeOption(opt);

    if (id) {

        // Check if the target component exists
        if (id >= isize(targets.size())) {
            warn("Invalid ID: %ld\n", *id);
            throw VC64Error(ERROR_OPT_INV_ID, "0..." + std::to_string(targets.size() - 1));
        }
        targets.at(*id)->setOption(opt, value);

    } else {

        // Configure all components
        for (const auto &target : targets) target->setOption(opt, value);
        return;
    }
}

void
Emulator::set(Option opt, const string &value)
{
    set(opt, OptionParser::create(opt)->parse(value));
}

void
Emulator::set(Option opt, const string &value, isize id)
{
    set(opt, OptionParser::create(opt)->parse(value), id);
}

void
Emulator::set(const string &opt, const string &value)
{
    set(Option(util::parseEnum<OptionEnum>(opt)), value);
}

void
Emulator::set(const string &opt, const string &value, isize id)
{
    set(Option(util::parseEnum<OptionEnum>(opt)), value, id);
}

void
Emulator::set(C64Model model)
{
    assert_enum(C64Model, model);

    {   SUSPENDED

        revertToFactorySettings();

        switch(model) {

            case C64_MODEL_PAL:

                set(OPT_VICII_REVISION, VICII_PAL_6569_R3);
                set(OPT_VICII_GRAY_DOT_BUG, false);
                set(OPT_CIA_REVISION, MOS_6526);
                set(OPT_CIA_TIMER_B_BUG, true);
                set(OPT_SID_REVISION, MOS_6581);
                set(OPT_SID_FILTER, true);
                set(OPT_POWER_GRID, GRID_STABLE_50HZ);
                set(OPT_GLUE_LOGIC, GLUE_LOGIC_DISCRETE);
                set(OPT_MEM_INIT_PATTERN, RAM_PATTERN_VICE);
                break;

            case C64_MODEL_PAL_II:

                set(OPT_VICII_REVISION, VICII_PAL_8565);
                set(OPT_VICII_GRAY_DOT_BUG, true);
                set(OPT_CIA_REVISION, MOS_8521);
                set(OPT_CIA_TIMER_B_BUG, false);
                set(OPT_SID_REVISION, MOS_8580);
                set(OPT_SID_FILTER, true);
                set(OPT_POWER_GRID, GRID_STABLE_50HZ);
                set(OPT_GLUE_LOGIC, GLUE_LOGIC_IC);
                set(OPT_MEM_INIT_PATTERN, RAM_PATTERN_VICE);
                break;

            case C64_MODEL_PAL_OLD:

                set(OPT_VICII_REVISION, VICII_PAL_6569_R1);
                set(OPT_VICII_GRAY_DOT_BUG, false);
                set(OPT_CIA_REVISION, MOS_6526);
                set(OPT_CIA_TIMER_B_BUG, true);
                set(OPT_SID_REVISION, MOS_6581);
                set(OPT_SID_FILTER, true);
                set(OPT_POWER_GRID, GRID_STABLE_50HZ);
                set(OPT_GLUE_LOGIC, GLUE_LOGIC_DISCRETE);
                set(OPT_MEM_INIT_PATTERN, RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC:

                set(OPT_VICII_REVISION, VICII_NTSC_6567);
                set(OPT_VICII_GRAY_DOT_BUG, false);
                set(OPT_CIA_REVISION, MOS_6526);
                set(OPT_CIA_TIMER_B_BUG, false);
                set(OPT_SID_REVISION, MOS_6581);
                set(OPT_SID_FILTER, true);
                set(OPT_POWER_GRID, GRID_STABLE_60HZ);
                set(OPT_GLUE_LOGIC, GLUE_LOGIC_DISCRETE);
                set(OPT_MEM_INIT_PATTERN, RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC_II:

                set(OPT_VICII_REVISION, VICII_NTSC_8562);
                set(OPT_VICII_GRAY_DOT_BUG, true);
                set(OPT_CIA_REVISION, MOS_8521);
                set(OPT_CIA_TIMER_B_BUG, true);
                set(OPT_SID_REVISION, MOS_8580);
                set(OPT_SID_FILTER, true);
                set(OPT_POWER_GRID, GRID_STABLE_60HZ);
                set(OPT_GLUE_LOGIC, GLUE_LOGIC_IC);
                set(OPT_MEM_INIT_PATTERN, RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC_OLD:

                set(OPT_VICII_REVISION, VICII_NTSC_6567_R56A);
                set(OPT_VICII_GRAY_DOT_BUG, false);
                set(OPT_CIA_REVISION, MOS_6526);
                set(OPT_CIA_TIMER_B_BUG, false);
                set(OPT_SID_REVISION, MOS_6581);
                set(OPT_SID_FILTER, true);
                set(OPT_POWER_GRID, GRID_STABLE_60HZ);
                set(OPT_GLUE_LOGIC, GLUE_LOGIC_DISCRETE);
                set(OPT_MEM_INIT_PATTERN, RAM_PATTERN_VICE);
                break;

            default:
                fatalError;
        }
    }
}

std::vector<const Configurable *>
Emulator::routeOption(Option opt) const
{
    std::vector<const Configurable *> result;

    for (const auto &target : const_cast<Emulator *>(this)->routeOption(opt)) {
        result.push_back(const_cast<const Configurable *>(target));
    }
    return result;
}

std::vector<Configurable *>
Emulator::routeOption(Option opt)
{
    switch (opt) {

        case OPT_EMU_WARP_MODE:
        case OPT_EMU_WARP_BOOT:
        case OPT_EMU_VSYNC:
        case OPT_EMU_SPEED_ADJUST:
        case OPT_EMU_SNAPSHOTS:
        case OPT_EMU_SNAPSHOT_DELAY:
        case OPT_EMU_RUN_AHEAD:

            return { this };

        case OPT_HOST_SAMPLE_RATE:
        case OPT_HOST_REFRESH_RATE:
        case OPT_HOST_FRAMEBUF_WIDTH:
        case OPT_HOST_FRAMEBUF_HEIGHT:

            return { &host };

        case OPT_VICII_REVISION:
        case OPT_VICII_GRAY_DOT_BUG:
        case OPT_VICII_POWER_SAVE:
        case OPT_VICII_HIDE_SPRITES:
        case OPT_VICII_SS_COLLISIONS:
        case OPT_VICII_SB_COLLISIONS:
        case OPT_GLUE_LOGIC:

            return { &main.vic };

        case OPT_MON_PALETTE:
        case OPT_MON_BRIGHTNESS:
        case OPT_MON_CONTRAST:
        case OPT_MON_SATURATION:
        case OPT_MON_HCENTER:
        case OPT_MON_VCENTER:
        case OPT_MON_HZOOM:
        case OPT_MON_VZOOM:
        case OPT_MON_UPSCALER:
        case OPT_MON_BLUR:
        case OPT_MON_BLUR_RADIUS:
        case OPT_MON_BLOOM:
        case OPT_MON_BLOOM_RADIUS:
        case OPT_MON_BLOOM_BRIGHTNESS:
        case OPT_MON_BLOOM_WEIGHT:
        case OPT_MON_DOTMASK:
        case OPT_MON_DOTMASK_BRIGHTNESS:
        case OPT_MON_SCANLINES:
        case OPT_MON_SCANLINE_BRIGHTNESS:
        case OPT_MON_SCANLINE_WEIGHT:
        case OPT_MON_DISALIGNMENT:
        case OPT_MON_DISALIGNMENT_H:
        case OPT_MON_DISALIGNMENT_V:

            return { &main.monitor };
            break;

        case OPT_VICII_CUT_LAYERS:
        case OPT_VICII_CUT_OPACITY:
        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_MODE:
        case OPT_DMA_DEBUG_OPACITY:
        case OPT_DMA_DEBUG_CHANNEL0:
        case OPT_DMA_DEBUG_CHANNEL1:
        case OPT_DMA_DEBUG_CHANNEL2:
        case OPT_DMA_DEBUG_CHANNEL3:
        case OPT_DMA_DEBUG_CHANNEL4:
        case OPT_DMA_DEBUG_CHANNEL5:
        case OPT_DMA_DEBUG_COLOR0:
        case OPT_DMA_DEBUG_COLOR1:
        case OPT_DMA_DEBUG_COLOR2:
        case OPT_DMA_DEBUG_COLOR3:
        case OPT_DMA_DEBUG_COLOR4:
        case OPT_DMA_DEBUG_COLOR5:

            return { &main.vic.dmaDebugger };
            break;

        case OPT_VID_WHITE_NOISE:

            return { &main.videoPort };
            break;

        case OPT_POWER_GRID:

            return { &main.supply };

        case OPT_CIA_REVISION:
        case OPT_CIA_TIMER_B_BUG:

            return { &main.cia1, &main.cia2 };

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_SID_POWER_SAVE:

            return { &main.sidBridge.sid[0],
                &main.sidBridge.sid[1],
                &main.sidBridge.sid[2],
                &main.sidBridge.sid[3] };

        case OPT_AUD_VOL0:
        case OPT_AUD_VOL1:
        case OPT_AUD_VOL2:
        case OPT_AUD_VOL3:
        case OPT_AUD_PAN0:
        case OPT_AUD_PAN1:
        case OPT_AUD_PAN2:
        case OPT_AUD_PAN3:
        case OPT_AUD_VOL_L:
        case OPT_AUD_VOL_R:

            return { &main.audioPort };

        case OPT_MEM_INIT_PATTERN:
        case OPT_MEM_HEATMAP:
        case OPT_MEM_SAVE_ROMS:

            return { &main.mem };

        case OPT_DRV_AUTO_CONFIG:
        case OPT_DRV_TYPE:
        case OPT_DRV_RAM:
        case OPT_DRV_SAVE_ROMS:
        case OPT_DRV_PARCABLE:
        case OPT_DRV_CONNECT:
        case OPT_DRV_POWER_SWITCH:
        case OPT_DRV_POWER_SAVE:
        case OPT_DRV_EJECT_DELAY:
        case OPT_DRV_SWAP_DELAY:
        case OPT_DRV_INSERT_DELAY:
        case OPT_DRV_PAN:
        case OPT_DRV_POWER_VOL:
        case OPT_DRV_STEP_VOL:
        case OPT_DRV_INSERT_VOL:
        case OPT_DRV_EJECT_VOL:

            return { &main.drive8, &main.drive9 };

        case OPT_DAT_MODEL:
        case OPT_DAT_CONNECT:

            return { &main.datasette };

        case OPT_MOUSE_MODEL:
        case OPT_MOUSE_SHAKE_DETECT:
        case OPT_MOUSE_VELOCITY:

            return { &main.port1.mouse, &main.port2.mouse };

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BURSTS:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            return { &main.port1.joystick, &main.port2.joystick };

        case OPT_REC_FRAME_RATE:
        case OPT_REC_BIT_RATE:
        case OPT_REC_SAMPLE_RATE:
        case OPT_REC_ASPECT_X:
        case OPT_REC_ASPECT_Y:

            return { &main.recorder };

        default:
            warn("Unrecognized option: %s\n", OptionEnum::key(opt));
            fatalError;
    }
}

i64
Emulator::overrideOption(Option opt, i64 value) const
{
    static std::map<Option,i64> overrides = OVERRIDES;

    if (overrides.find(opt) != overrides.end()) {

        msg("Overriding option: %s = %lld\n", OptionEnum::key(opt), value);
        return overrides[opt];
    }

    return value;
}

}
