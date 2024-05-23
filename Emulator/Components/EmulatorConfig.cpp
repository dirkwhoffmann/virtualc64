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
        case OPT_EMU_TIME_LAPSE:        return config.timeLapse;
        case OPT_EMU_SNAPSHOTS:         return config.snapshots;
        case OPT_EMU_SNAPSHOT_DELAY:    return config.snapshotDelay;
        case OPT_EMU_RUN_AHEAD:         return config.runAhead;

        default:
            fatalError;
    }
}

void
Emulator::setOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_EMU_WARP_BOOT:

            config.warpBoot = isize(value);
            return;

        case OPT_EMU_WARP_MODE:

            if (!WarpModeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, WarpModeEnum::keyList());
            }

            config.warpMode = WarpMode(value);
            return;

        case OPT_EMU_VSYNC:

            config.vsync = bool(value);
            return;

        case OPT_EMU_TIME_LAPSE:

            if (value < 50 || value > 200) {
                throw VC64Error(ERROR_OPT_INVARG, "50...200");
            }

            config.timeLapse = isize(value);
            return;

        case OPT_EMU_SNAPSHOTS:

            config.snapshots = bool(value);
            main.scheduleNextSNPEvent();
            return;

        case OPT_EMU_SNAPSHOT_DELAY:

            if (value < 10 || value > 3600) {
                throw VC64Error(ERROR_OPT_INVARG, "10...3600");
            }

            config.snapshotDelay = isize(value);
            main.scheduleNextSNPEvent();
            return;

        case OPT_EMU_RUN_AHEAD:

            if (value < 0 || value > 12) {
                throw VC64Error(ERROR_OPT_INVARG, "0...12");
            }

            config.runAhead = isize(value);
            return;

        default:
            fatalError;
    }
}

i64
Emulator::get(Option option) const
{
    switch (option) {

        case OPT_EMU_WARP_BOOT:
        case OPT_EMU_WARP_MODE:
        case OPT_EMU_VSYNC:
        case OPT_EMU_TIME_LAPSE:
        case OPT_EMU_SNAPSHOTS:
        case OPT_EMU_SNAPSHOT_DELAY:
        case OPT_EMU_RUN_AHEAD:

            return getOption(option);

        case OPT_HOST_REFRESH_RATE:
        case OPT_HOST_SAMPLE_RATE:
        case OPT_HOST_FRAMEBUF_WIDTH:
        case OPT_HOST_FRAMEBUF_HEIGHT:

            return host.getOption(option);

        case OPT_VICII_REVISION:
        case OPT_VICII_POWER_SAVE:
        case OPT_VICII_GRAY_DOT_BUG:
        case OPT_GLUE_LOGIC:
        case OPT_VICII_HIDE_SPRITES:
        case OPT_VICII_SS_COLLISIONS:
        case OPT_VICII_SB_COLLISIONS:

            return main.vic.getOption(option);

        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_MODE:
        case OPT_DMA_DEBUG_OPACITY:
        case OPT_VICII_CUT_LAYERS:
        case OPT_VICII_CUT_OPACITY:

            return main.vic.dmaDebugger.getOption(option);

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

            return main.monitor.getOption(option);

        case OPT_CIA_REVISION:
        case OPT_CIA_TIMER_B_BUG:

            assert(main.cia1.getOption(option) == main.cia2.getOption(option));
            return main.cia1.getOption(option);

        case OPT_POWER_GRID:

            return main.supply.getOption(option);

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

            return main.audioPort.getOption(option);

        case OPT_RAM_PATTERN:
        case OPT_SAVE_ROMS:

            return main.mem.getOption(option);

        case OPT_DAT_MODEL:
        case OPT_DAT_CONNECT:

            return main.datasette.getOption(option);

        case OPT_REC_FRAME_RATE:
        case OPT_REC_BIT_RATE:
        case OPT_REC_SAMPLE_RATE:
        case OPT_REC_ASPECT_X:
        case OPT_REC_ASPECT_Y:

            return main.recorder.getOption(option);

        default:
            fatalError;
    }
}

i64
Emulator::get(Option option, isize id) const
{
    switch (option) {

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_SID_POWER_SAVE:

            if (id == 0) return main.sidBridge.sid[0].getOption(option);
            if (id == 1) return main.sidBridge.sid[1].getOption(option);
            if (id == 2) return main.sidBridge.sid[2].getOption(option);
            if (id == 3) return main.sidBridge.sid[3].getOption(option);
            fatalError;

        case OPT_DRV_CONNECT:
        case OPT_DRV_AUTO_CONFIG:
        case OPT_DRV_TYPE:
        case OPT_DRV_RAM:
        case OPT_DRV_SAVE_ROMS:
        case OPT_DRV_PARCABLE:
        case OPT_DRV_POWER_SAVE:
        case OPT_DRV_POWER_SWITCH:
        case OPT_DRV_EJECT_DELAY:
        case OPT_DRV_SWAP_DELAY:
        case OPT_DRV_INSERT_DELAY:
        case OPT_DRV_PAN:
        case OPT_DRV_POWER_VOL:
        case OPT_DRV_STEP_VOL:
        case OPT_DRV_INSERT_VOL:
        case OPT_DRV_EJECT_VOL:

            if (id == DRIVE8) return main.drive8.getOption(option);
            if (id == DRIVE9) return main.drive9.getOption(option);
            fatalError;

        case OPT_MOUSE_MODEL:
        case OPT_MOUSE_SHAKE_DETECT:
        case OPT_MOUSE_VELOCITY:

            if (id == PORT_1) return main.port1.mouse.getOption(option);
            if (id == PORT_2) return main.port2.mouse.getOption(option);
            fatalError;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_BURSTS:
        case OPT_AUTOFIRE_DELAY:

            if (id == PORT_1) return main.port1.joystick.getOption(option);
            if (id == PORT_2) return main.port2.joystick.getOption(option);
            fatalError;

        default:
            fatalError;
    }
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
                set(OPT_CIA_TIMER_B_BUG,  true);
                set(OPT_SID_REVISION, MOS_6581);
                set(OPT_SID_FILTER,   true);
                set(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                set(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                set(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_PAL_II:

                set(OPT_VICII_REVISION, VICII_PAL_8565);
                set(OPT_VICII_GRAY_DOT_BUG, true);
                set(OPT_CIA_REVISION, MOS_8521);
                set(OPT_CIA_TIMER_B_BUG,  false);
                set(OPT_SID_REVISION, MOS_8580);
                set(OPT_SID_FILTER,   true);
                set(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                set(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
                set(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_PAL_OLD:

                set(OPT_VICII_REVISION, VICII_PAL_6569_R1);
                set(OPT_VICII_GRAY_DOT_BUG, false);
                set(OPT_CIA_REVISION, MOS_6526);
                set(OPT_CIA_TIMER_B_BUG,  true);
                set(OPT_SID_REVISION, MOS_6581);
                set(OPT_SID_FILTER,   true);
                set(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                set(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                set(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC:

                set(OPT_VICII_REVISION, VICII_NTSC_6567);
                set(OPT_VICII_GRAY_DOT_BUG, false);
                set(OPT_CIA_REVISION, MOS_6526);
                set(OPT_CIA_TIMER_B_BUG,  false);
                set(OPT_SID_REVISION, MOS_6581);
                set(OPT_SID_FILTER,   true);
                set(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                set(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                set(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC_II:

                set(OPT_VICII_REVISION, VICII_NTSC_8562);
                set(OPT_VICII_GRAY_DOT_BUG, true);
                set(OPT_CIA_REVISION, MOS_8521);
                set(OPT_CIA_TIMER_B_BUG,  true);
                set(OPT_SID_REVISION, MOS_8580);
                set(OPT_SID_FILTER,   true);
                set(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                set(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
                set(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC_OLD:

                set(OPT_VICII_REVISION, VICII_NTSC_6567_R56A);
                set(OPT_VICII_GRAY_DOT_BUG, false);
                set(OPT_CIA_REVISION, MOS_6526);
                set(OPT_CIA_TIMER_B_BUG,  false);
                set(OPT_SID_REVISION, MOS_6581);
                set(OPT_SID_FILTER,   true);
                set(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                set(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                set(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            default:
                fatalError;
        }
    }
}

void
Emulator::set(Option option, i64 value)
{
    debug(CNF_DEBUG, "configure(%s, %lld)\n", OptionEnum::key(option), value);

    if (!isInitialized()) initialize();

    // The following options do not send a message to the GUI
    static std::vector<Option> quiet = {

        OPT_MON_BRIGHTNESS,
        OPT_MON_CONTRAST,
        OPT_MON_SATURATION,
        OPT_VICII_CUT_OPACITY,
        OPT_DMA_DEBUG_OPACITY,
        OPT_MOUSE_VELOCITY,
        OPT_AUTOFIRE_DELAY,
        OPT_AUD_VOL0,
        OPT_AUD_VOL1,
        OPT_AUD_VOL2,
        OPT_AUD_VOL3,
        OPT_AUD_PAN0,
        OPT_AUD_PAN1,
        OPT_AUD_PAN2,
        OPT_AUD_PAN3,
        OPT_AUD_VOL_L,
        OPT_AUD_VOL_R,
        OPT_DRV_PAN,
        OPT_DRV_POWER_VOL,
        OPT_DRV_STEP_VOL,
        OPT_DRV_INSERT_VOL,
        OPT_DRV_EJECT_VOL
    };

    // Check if this option has been locked for debugging
    value = overrideOption(option, value);

    switch (option) {

        case OPT_EMU_WARP_MODE:
        case OPT_EMU_WARP_BOOT:
        case OPT_EMU_VSYNC:
        case OPT_EMU_TIME_LAPSE:
        case OPT_EMU_SNAPSHOTS:
        case OPT_EMU_SNAPSHOT_DELAY:
        case OPT_EMU_RUN_AHEAD:

            setOption(option, value);
            break;

        case OPT_HOST_SAMPLE_RATE:
        case OPT_HOST_REFRESH_RATE:
        case OPT_HOST_FRAMEBUF_WIDTH:
        case OPT_HOST_FRAMEBUF_HEIGHT:

            host.setOption(option, value);
            break;

        case OPT_VICII_REVISION:
        case OPT_VICII_GRAY_DOT_BUG:
        case OPT_VICII_POWER_SAVE:
        case OPT_VICII_HIDE_SPRITES:
        case OPT_VICII_SS_COLLISIONS:
        case OPT_VICII_SB_COLLISIONS:
        case OPT_GLUE_LOGIC:

            main.vic.setOption(option, value);
            break;

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

            main.monitor.setOption(option, value);
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

            main.vic.dmaDebugger.setOption(option, value);
            break;

        case OPT_POWER_GRID:

            main.supply.setOption(option, value);
            break;

        case OPT_CIA_REVISION:
        case OPT_CIA_TIMER_B_BUG:

            main.cia1.setOption(option, value);
            main.cia2.setOption(option, value);
            break;

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_SID_POWER_SAVE:

            main.sidBridge.sid[0].setOption(option, value);
            main.sidBridge.sid[1].setOption(option, value);
            main.sidBridge.sid[2].setOption(option, value);
            main.sidBridge.sid[3].setOption(option, value);
            break;

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

            main.audioPort.setOption(option, value);
            break;

        case OPT_RAM_PATTERN:
        case OPT_SAVE_ROMS:

            main.mem.setOption(option, value);
            main.mem.setOption(option, value);
            break;

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

            main.drive8.setOption(option, value);
            main.drive9.setOption(option, value);
            break;

        case OPT_DAT_MODEL:
        case OPT_DAT_CONNECT:

            main.datasette.setOption(option, value);

        case OPT_MOUSE_MODEL:
        case OPT_MOUSE_SHAKE_DETECT:
        case OPT_MOUSE_VELOCITY:

            main.port1.mouse.setOption(option, value);
            main.port2.mouse.setOption(option, value);
            break;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BURSTS:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            main.port1.joystick.setOption(option, value);
            main.port2.joystick.setOption(option, value);
            break;

        case OPT_REC_FRAME_RATE:
        case OPT_REC_BIT_RATE:
        case OPT_REC_SAMPLE_RATE:
        case OPT_REC_ASPECT_X:
        case OPT_REC_ASPECT_Y:

            main.recorder.setOption(option, value);
            break;

        default:
            warn("Unrecognized option: %s\n", OptionEnum::key(option));
            fatalError;
    }

    if (std::find(quiet.begin(), quiet.end(), option) == quiet.end()) {
        main.msgQueue.put(MSG_CONFIG, option);
    }
}

void
Emulator::set(Option option, isize id, i64 value)
{
    debug(CNF_DEBUG, "configure(%s, %ld, %lld)\n", OptionEnum::key(option), id, value);

    if (!isInitialized()) initialize();

    // Check if this option has been locked for debugging
    value = overrideOption(option, value);

    // The following options do not send a message to the GUI
    static std::vector<Option> quiet = {

        OPT_MOUSE_VELOCITY,
        OPT_AUTOFIRE_DELAY,
        OPT_AUD_VOL0,
        OPT_AUD_VOL1,
        OPT_AUD_VOL2,
        OPT_AUD_VOL3,
        OPT_AUD_PAN0,
        OPT_AUD_PAN1,
        OPT_AUD_PAN2,
        OPT_AUD_PAN3,
        OPT_AUD_VOL_L,
        OPT_AUD_VOL_R,
        OPT_DRV_PAN,
        OPT_DRV_POWER_VOL,
        OPT_DRV_STEP_VOL,
        OPT_DRV_INSERT_VOL,
        OPT_DRV_EJECT_VOL
    };

    switch (option) {

        case OPT_CIA_REVISION:
        case OPT_CIA_TIMER_B_BUG:

            switch (id) {
                case 0: main.cia1.setOption(option, value); break;
                case 1: main.cia2.setOption(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_MOUSE_MODEL:
        case OPT_MOUSE_SHAKE_DETECT:
        case OPT_MOUSE_VELOCITY:

            switch (id) {
                case PORT_1: main.port1.mouse.setOption(option, value); break;
                case PORT_2: main.port2.mouse.setOption(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BURSTS:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            switch (id) {
                case PORT_1: main.port1.joystick.setOption(option, value); break;
                case PORT_2: main.port2.joystick.setOption(option, value); break;
                default: fatalError;
            }
            break;

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

            main.audioPort.setOption(option, value);
            break;

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_SID_POWER_SAVE:

            main.sidBridge.sid[id].setOption(option, value);
            break;

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

            switch (id) {
                case DRIVE8: main.drive8.setOption(option, value); break;
                case DRIVE9: main.drive9.setOption(option, value); break;
                default: fatalError;
            }
            break;

        default:
            warn("Unrecognized option: %s\n", OptionEnum::key(option));
            fatalError;
    }

    if (std::find(quiet.begin(), quiet.end(), option) == quiet.end()) {
        main.msgQueue.put(MSG_CONFIG, option);
    }
}

void
Emulator::set(Option opt, const string &value)
{
    set(opt, OptionParser::create(opt)->parse(value));
}

void
Emulator::set(Option opt, isize id, const string &value)
{
    set(opt, id, OptionParser::create(opt)->parse(value));
}

void
Emulator::set(const string &opt, const string &value)
{
    set(Option(util::parseEnum<OptionEnum>(opt)), value);
}

void
Emulator::set(const string &opt, isize id, const string &value)
{
    set(Option(util::parseEnum<OptionEnum>(opt)), id, value);
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

}
