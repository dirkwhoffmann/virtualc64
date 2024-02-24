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

Emulator::Emulator()
{

}

Emulator::~Emulator()
{

}

void
Emulator::launch(const void *listener, Callback *func)
{
    // Add listener
    main.msgQueue.setListener(listener, func);

    // Run the initialization procedure
    initialize();

    // Launch the emulator thread
    Thread::launch();
}

void
Emulator::initialize()
{
    // Initialize all components
    resetConfig();
    main.initialize();
    ahead.initialize();

    // Perform a hard reset
    main.hardReset();
    ahead.hardReset();
}

void 
Emulator::recordState(EmulatorInfo &result) const
{
    result.state = state;
    result.refreshRate = isize(refreshRate());
    result.powered = isPoweredOn();
    result.paused = isPaused();
    result.running = isRunning();
    result.suspended = isSuspended();
    result.warping = isWarping();
    result.tracking = isTracking();
}

void
Emulator::recordStats(EmulatorStats &result) const
{
    result.cpuLoad = cpuLoad;
    result.fps = fps;    
}

void
Emulator::revertToFactorySettings()
{
    // Power off the emulator
    powerOff();

    // Put all components into their initial state
    initialize();
}

void
Emulator::resetConfig()
{
    Configurable::resetConfig(defaults);
}

i64 
Emulator::getOption(Option opt) const
{
    switch (opt) {

        case OPT_EMU_WARP_BOOT:     return config.warpBoot;
        case OPT_EMU_WARP_MODE:     return config.warpMode;
        case OPT_EMU_VSYNC:         return config.vsync;
        case OPT_EMU_TIME_LAPSE:    return config.timeLapse;
        case OPT_EMU_RUN_AHEAD:     return config.runAhead;

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

void
Emulator::configure(Option option, i64 value)
{
    debug(CNF_DEBUG, "configure(%s, %lld)\n", OptionEnum::key(option), value);

    // The following options do not send a message to the GUI
    static std::vector<Option> quiet = {

        OPT_VICII_BRIGHTNESS,
        OPT_VICII_CONTRAST,
        OPT_VICII_SATURATION,
        OPT_VICII_CUT_OPACITY,
        OPT_DMA_DEBUG_OPACITY,
        OPT_MOUSE_VELOCITY,
        OPT_JOY_AUTOFIRE_DELAY,
        OPT_AUD_PAN,
        OPT_AUD_VOL,
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
        case OPT_EMU_RUN_AHEAD:

            setOption(option, value);
            break;

        case OPT_HOST_SAMPLE_RATE:

            host.setOption(option, value);
            main.muxer.setOption(option, value);
            break;

        case OPT_HOST_REFRESH_RATE:
        case OPT_HOST_FRAMEBUF_WIDTH:
        case OPT_HOST_FRAMEBUF_HEIGHT:

            host.setOption(option, value);
            break;

        case OPT_VICII_REVISION:
        case OPT_VICII_PALETTE:
        case OPT_VICII_BRIGHTNESS:
        case OPT_VICII_CONTRAST:
        case OPT_VICII_SATURATION:
        case OPT_VICII_GRAY_DOT_BUG:
        case OPT_VICII_POWER_SAVE:
        case OPT_VICII_HIDE_SPRITES:
        case OPT_VICII_SS_COLLISIONS:
        case OPT_VICII_SB_COLLISIONS:
        case OPT_GLUE_LOGIC:

            main.vic.setOption(option, value);
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

            main.muxer.sid[0].setOption(option, value);
            main.muxer.sid[1].setOption(option, value);
            main.muxer.sid[2].setOption(option, value);
            main.muxer.sid[3].setOption(option, value);

        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_SAMPLING:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_ENGINE:
        case OPT_AUD_PAN:
        case OPT_AUD_VOL:
        case OPT_AUD_VOL_L:
        case OPT_AUD_VOL_R:

            main.muxer.setOption(option, value);
            break;

        case OPT_RAM_PATTERN:

            main.mem.setOption(option, value);
            break;

        case OPT_SAVE_ROMS:

            main.mem.setOption(option, value);
            main.drive8.mem.setOption(option, value);
            main.drive9.mem.setOption(option, value);
            break;

        case OPT_DRV_AUTO_CONFIG:
        case OPT_DRV_TYPE:
        case OPT_DRV_RAM:
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

        case OPT_JOY_AUTOFIRE:
        case OPT_JOY_AUTOFIRE_BULLETS:
        case OPT_JOY_AUTOFIRE_DELAY:

            main.port1.joystick.setOption(option, value);
            main.port2.joystick.setOption(option, value);
            break;

        default:
            warn("Unrecognized option: %s\n", OptionEnum::key(option));
            fatalError;
    }

    if (std::find(quiet.begin(), quiet.end(), option) == quiet.end()) {
        main.msgQueue.put(MSG_CONFIG, option);
    }
}

void Emulator::configure(Option option, const string &value)
{
    debug(CNF_DEBUG, "configure(%s, \"%s\")\n", OptionEnum::key(option), value.c_str());

    using namespace util;
    auto config = [&](std::function<i64(const string &)> func) { configure(option, func(value)); };

    switch (option) {

        case OPT_EMU_WARP_MODE:             return config(parseEnum<VICIIRevisionEnum>);
        case OPT_EMU_WARP_BOOT:             return config(parseBool);
        case OPT_EMU_VSYNC:                 return config(parseBool);
        case OPT_EMU_TIME_LAPSE:            return config(parseNum);
        case OPT_EMU_RUN_AHEAD:             return config(parseNum);

        case OPT_HOST_SAMPLE_RATE:          return config(parseNum);

        case OPT_HOST_REFRESH_RATE:         return config(parseNum);
        case OPT_HOST_FRAMEBUF_WIDTH:       return config(parseNum);
        case OPT_HOST_FRAMEBUF_HEIGHT:      return config(parseNum);

        case OPT_VICII_REVISION:            return config(parseEnum<VICIIRevisionEnum>);
        case OPT_VICII_PALETTE:             return config(parseEnum<PaletteEnum>);
        case OPT_VICII_BRIGHTNESS:          return config(parseNum);
        case OPT_VICII_CONTRAST:            return config(parseNum);
        case OPT_VICII_SATURATION:          return config(parseNum);
        case OPT_VICII_GRAY_DOT_BUG:        return config(parseBool);
        case OPT_VICII_POWER_SAVE:          return config(parseBool);
        case OPT_VICII_HIDE_SPRITES:        return config(parseBool);
        case OPT_VICII_SS_COLLISIONS:       return config(parseBool);
        case OPT_VICII_SB_COLLISIONS:       return config(parseBool);
        case OPT_GLUE_LOGIC:                return config(parseEnum<GlueLogicEnum>);

        case OPT_VICII_CUT_LAYERS:          return config(parseNum);
        case OPT_VICII_CUT_OPACITY:         return config(parseNum);
        case OPT_DMA_DEBUG_ENABLE:          return config(parseBool);
        case OPT_DMA_DEBUG_MODE:            return config(parseEnum<DmaDisplayModeEnum>);
        case OPT_DMA_DEBUG_OPACITY:         return config(parseNum);

        case OPT_POWER_GRID:                return config(parseEnum<PowerGridEnum>);

        case OPT_CIA_REVISION:              return config(parseEnum<CIARevisionEnum>);
        case OPT_CIA_TIMER_B_BUG:           return config(parseBool);

        case OPT_SID_ENABLE:                return config(parseNum);
        case OPT_SID_ADDRESS:               return config(parseNum);

        case OPT_SID_REVISION:              return config(parseEnum<SIDRevisionEnum>);
        case OPT_SID_FILTER:                return config(parseBool);
        case OPT_SID_SAMPLING:              return config(parseEnum<SamplingMethodEnum>);
        case OPT_SID_POWER_SAVE:            return config(parseBool);
        case OPT_SID_ENGINE:                return config(parseEnum<SIDEngineEnum>);
        case OPT_AUD_PAN:                   return config(parseNum);
        case OPT_AUD_VOL:                   return config(parseNum);
        case OPT_AUD_VOL_L:                 return config(parseNum);
        case OPT_AUD_VOL_R:                 return config(parseNum);

        case OPT_RAM_PATTERN:               return config(parseEnum<RamPatternEnum>);

        case OPT_SAVE_ROMS:                 return config(parseBool);

        case OPT_DRV_AUTO_CONFIG:           return config(parseBool);
        case OPT_DRV_TYPE:                  return config(parseEnum<DriveTypeEnum>);
        case OPT_DRV_RAM:                   return config(parseEnum<DriveRamEnum>);
        case OPT_DRV_PARCABLE:              return config(parseEnum<ParCableTypeEnum>);
        case OPT_DRV_CONNECT:               return config(parseBool);
        case OPT_DRV_POWER_SWITCH:          return config(parseBool);
        case OPT_DRV_POWER_SAVE:            return config(parseBool);
        case OPT_DRV_EJECT_DELAY:           return config(parseNum);
        case OPT_DRV_SWAP_DELAY:            return config(parseNum);
        case OPT_DRV_INSERT_DELAY:          return config(parseNum);
        case OPT_DRV_PAN:                   return config(parseNum);
        case OPT_DRV_POWER_VOL:             return config(parseNum);
        case OPT_DRV_STEP_VOL:              return config(parseNum);
        case OPT_DRV_INSERT_VOL:            return config(parseNum);
        case OPT_DRV_EJECT_VOL:             return config(parseNum);

        case OPT_DAT_MODEL:                 return config(parseEnum<DatasetteModelEnum>);
        case OPT_DAT_CONNECT:               return config(parseBool);

        case OPT_MOUSE_MODEL:               return config(parseEnum<MouseModelEnum>);
        case OPT_MOUSE_SHAKE_DETECT:        return config(parseBool);
        case OPT_MOUSE_VELOCITY:            return config(parseNum);

        case OPT_JOY_AUTOFIRE:              return config(parseBool);
        case OPT_JOY_AUTOFIRE_BULLETS:      return config(parseNum);
        case OPT_JOY_AUTOFIRE_DELAY:        return config(parseNum);

        default:
            warn("Unrecognized option: %s\n", OptionEnum::key(option));
            fatalError;
    }
}

void
Emulator::configure(Option option, long id, i64 value)
{
    debug(CNF_DEBUG, "configure(%s, %ld, %lld)\n", OptionEnum::key(option), id, value);

    // Check if this option has been locked for debugging
    value = overrideOption(option, value);

    // The following options do not send a message to the GUI
    static std::vector<Option> quiet = {

        OPT_MOUSE_VELOCITY,
        OPT_JOY_AUTOFIRE_DELAY,
        OPT_AUD_PAN,
        OPT_AUD_VOL,
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

        case OPT_JOY_AUTOFIRE:
        case OPT_JOY_AUTOFIRE_BULLETS:
        case OPT_JOY_AUTOFIRE_DELAY:

            switch (id) {
                case PORT_1: main.port1.joystick.setOption(option, value); break;
                case PORT_2: main.port2.joystick.setOption(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_AUD_VOL_L:
        case OPT_AUD_VOL_R:

            main.muxer.setOption(option, value);
            break;

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_AUD_PAN:
        case OPT_AUD_VOL:

            main.muxer.sid[id].setOption(option, value);
            break;

        case OPT_DRV_AUTO_CONFIG:
        case OPT_DRV_TYPE:
        case OPT_DRV_RAM:
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
Emulator::configure(Option option, long id, const string &value)
{
    debug(CNF_DEBUG, "configure(%s, %ld, \"%s\")\n", OptionEnum::key(option), id, value.c_str());

    using namespace util;
    auto config = [&](std::function<i64(const string &)> func) { configure(option, id, func(value)); };

    switch (option) {

        case OPT_DMA_DEBUG_ENABLE:      return config(parseBool);
        case OPT_DMA_DEBUG_CHANNEL0:    return config(parseBool);
        case OPT_DMA_DEBUG_CHANNEL1:    return config(parseBool);
        case OPT_DMA_DEBUG_CHANNEL2:    return config(parseBool);
        case OPT_DMA_DEBUG_CHANNEL3:    return config(parseBool);
        case OPT_DMA_DEBUG_CHANNEL4:    return config(parseBool);
        case OPT_DMA_DEBUG_CHANNEL5:    return config(parseBool);
        case OPT_DMA_DEBUG_COLOR0:      return config(parseNum);
        case OPT_DMA_DEBUG_COLOR1:      return config(parseNum);
        case OPT_DMA_DEBUG_COLOR2:      return config(parseNum);
        case OPT_DMA_DEBUG_COLOR3:      return config(parseNum);
        case OPT_DMA_DEBUG_COLOR4:      return config(parseNum);
        case OPT_DMA_DEBUG_COLOR5:      return config(parseNum);

        case OPT_CIA_REVISION:          return config(parseEnum<CIARevisionEnum>);
        case OPT_CIA_TIMER_B_BUG:       return config(parseBool);

        case OPT_MOUSE_MODEL:           return config(parseEnum<MouseModelEnum>);
        case OPT_MOUSE_SHAKE_DETECT:    return config(parseBool);
        case OPT_MOUSE_VELOCITY:        return config(parseNum);

        case OPT_JOY_AUTOFIRE:          return config(parseBool);
        case OPT_JOY_AUTOFIRE_BULLETS:  return config(parseNum);
        case OPT_JOY_AUTOFIRE_DELAY:    return config(parseNum);

        case OPT_SID_ENABLE:            return config(parseBool);
        case OPT_SID_ADDRESS:           return config(parseNum);
        case OPT_SID_REVISION:          return config(parseEnum<SIDRevisionEnum>);
        case OPT_SID_FILTER:            return config(parseBool);
        case OPT_SID_POWER_SAVE:        return config(parseBool);
        case OPT_SID_ENGINE:            return config(parseEnum<SIDEngineEnum>);
        case OPT_SID_SAMPLING:          return config(parseEnum<SamplingMethodEnum>);
        case OPT_AUD_PAN:               return config(parseNum);
        case OPT_AUD_VOL:               return config(parseNum);
        case OPT_AUD_VOL_L:             return config(parseNum);
        case OPT_AUD_VOL_R:             return config(parseNum);

        case OPT_DRV_AUTO_CONFIG:       return config(parseBool);
        case OPT_DRV_TYPE:              return config(parseEnum<DriveTypeEnum>);
        case OPT_DRV_RAM:               return config(parseEnum<DriveRamEnum>);
        case OPT_DRV_PARCABLE:          return config(parseEnum<ParCableTypeEnum>);
        case OPT_DRV_CONNECT:           return config(parseBool);
        case OPT_DRV_POWER_SWITCH:      return config(parseBool);
        case OPT_DRV_POWER_SAVE:        return config(parseBool);
        case OPT_DRV_EJECT_DELAY:       return config(parseNum);
        case OPT_DRV_SWAP_DELAY:        return config(parseNum);
        case OPT_DRV_INSERT_DELAY:      return config(parseNum);
        case OPT_DRV_PAN:               return config(parseNum);
        case OPT_DRV_POWER_VOL:         return config(parseNum);
        case OPT_DRV_STEP_VOL:          return config(parseNum);
        case OPT_DRV_INSERT_VOL:        return config(parseNum);
        case OPT_DRV_EJECT_VOL:         return config(parseNum);

        default:
            warn("Unrecognized option: %s\n", OptionEnum::key(option));
            fatalError;
    }
}

void
Emulator::configure(C64Model model)
{
    assert_enum(C64Model, model);

    {   SUSPENDED

        revertToFactorySettings();

        switch(model) {

            case C64_MODEL_PAL:

                configure(OPT_VICII_REVISION, VICII_PAL_6569_R3);
                configure(OPT_VICII_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_CIA_TIMER_B_BUG,  true);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_PAL_II:

                configure(OPT_VICII_REVISION, VICII_PAL_8565);
                configure(OPT_VICII_GRAY_DOT_BUG, true);
                configure(OPT_CIA_REVISION, MOS_8521);
                configure(OPT_CIA_TIMER_B_BUG,  false);
                configure(OPT_SID_REVISION, MOS_8580);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_PAL_OLD:

                configure(OPT_VICII_REVISION, VICII_PAL_6569_R1);
                configure(OPT_VICII_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_CIA_TIMER_B_BUG,  true);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC:

                configure(OPT_VICII_REVISION, VICII_NTSC_6567);
                configure(OPT_VICII_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_CIA_TIMER_B_BUG,  false);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC_II:

                configure(OPT_VICII_REVISION, VICII_NTSC_8562);
                configure(OPT_VICII_GRAY_DOT_BUG, true);
                configure(OPT_CIA_REVISION, MOS_8521);
                configure(OPT_CIA_TIMER_B_BUG,  true);
                configure(OPT_SID_REVISION, MOS_8580);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC_OLD:

                configure(OPT_VICII_REVISION, VICII_NTSC_6567_R56A);
                configure(OPT_VICII_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_CIA_TIMER_B_BUG,  false);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            default:
                fatalError;
        }
    }
}

i64
Emulator::getConfigItem(Option option) const
{
    switch (option) {

        case OPT_EMU_WARP_BOOT:     return config.warpBoot;
        case OPT_EMU_WARP_MODE:     return config.warpMode;
        case OPT_EMU_VSYNC:         return config.vsync;
        case OPT_EMU_TIME_LAPSE:    return config.timeLapse;
        case OPT_EMU_RUN_AHEAD:     return config.runAhead;

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

        case OPT_VICII_PALETTE:
        case OPT_VICII_BRIGHTNESS:
        case OPT_VICII_CONTRAST:
        case OPT_VICII_SATURATION:

            return main.vic.getOption(option);

        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_MODE:
        case OPT_DMA_DEBUG_OPACITY:
        case OPT_VICII_CUT_LAYERS:
        case OPT_VICII_CUT_OPACITY:

            return main.vic.dmaDebugger.getOption(option);

        case OPT_CIA_REVISION:
        case OPT_CIA_TIMER_B_BUG:

            assert(main.cia1.getOption(option) == main.cia2.getOption(option));
            return main.cia1.getOption(option);

        case OPT_POWER_GRID:

            return main.supply.getOption(option);

        case OPT_SID_REVISION:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_FILTER:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_AUD_VOL_L:
        case OPT_AUD_VOL_R:

            return main.muxer.getOption(option);

        case OPT_RAM_PATTERN:
        case OPT_SAVE_ROMS:

            return main.mem.getOption(option);

        case OPT_DAT_MODEL:
        case OPT_DAT_CONNECT:

            return main.datasette.getOption(option);

        default:
            fatalError;
    }
}

i64
Emulator::getConfigItem(Option option, long id) const
{
    const Drive &drive = id == DRIVE8 ? main.drive8 : main.drive9;

    switch (option) {

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_AUD_PAN:
        case OPT_AUD_VOL:

            if (id == 0) return main.muxer.sid[0].getOption(option);
            if (id == 1) return main.muxer.sid[1].getOption(option);
            if (id == 2) return main.muxer.sid[2].getOption(option);
            if (id == 3) return main.muxer.sid[3].getOption(option);
            fatalError;

        case OPT_DRV_CONNECT:
        case OPT_DRV_AUTO_CONFIG:
        case OPT_DRV_TYPE:
        case OPT_DRV_RAM:
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

            return drive.getOption(option);

        case OPT_MOUSE_MODEL:
        case OPT_MOUSE_SHAKE_DETECT:
        case OPT_MOUSE_VELOCITY:

            if (id == PORT_1) return main.port1.mouse.getOption(option);
            if (id == PORT_2) return main.port2.mouse.getOption(option);
            fatalError;

        case OPT_JOY_AUTOFIRE:
        case OPT_JOY_AUTOFIRE_BULLETS:
        case OPT_JOY_AUTOFIRE_DELAY:

            if (id == PORT_1) return main.port1.joystick.getOption(option);
            if (id == PORT_2) return main.port2.joystick.getOption(option);
            fatalError;

        default:
            fatalError;
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

    if (category == Category::Debug) {

        for (isize i = DebugFlagEnum::minVal; i < DebugFlagEnum::maxVal; i++) {

            os << tab(DebugFlagEnum::key(i));
            os << bol(main.getDebugVariable(i)) << std::endl;
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

        os << tab("Frame");
        os << dec(ahead.frame) << std::endl;
        os << tab("Beam");
        os << " (" << dec(ahead.scanline) << "," << dec(ahead.rasterCycle) << ")" << std::endl;
        os << tab("Cycle");
        os << dec(ahead.cpu.clock) << std::endl;
    }

    if (category == Category::State) {

        os << tab("Emulator state");
        os << EmulatorStateEnum::key(state) << std::endl;
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

void
Emulator::isReady()
{
    main.isReady();
}

bool
Emulator::shouldWarp()
{
    if (main.cpu.clock < SEC(config.warpBoot)) {

        return true;

    } else {

        switch (config.warpMode) {

            case WARP_AUTO:     return main.iec.isTransferring();
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
Emulator::update()
{
    shouldWarp() ? warpOn() : warpOff();

    Cmd cmd;
    while (cmdQueue.poll(cmd)) {

        debug(CMD_DEBUG, "Command: %s\n", CmdTypeEnum::key(cmd.type));

        main.markAsDirty();

        switch (cmd.type) {

            case CMD_POWER_ON:  powerOn();  break;
            case CMD_POWER_OFF: powerOff(); break;
            case CMD_RUN:       run();      break;
            case CMD_PAUSE:     pause();    break;
            case CMD_SUSPEND:   suspend();  break;
            case CMD_RESUME:    resume();   break;
            case CMD_HALT:      halt();     break;

            case CMD_BRK:
            case CMD_SNAPSHOT_AUTO:
            case CMD_SNAPSHOT_USER: 

                main.process(cmd);
                break;

            case CMD_KEY_PRESS:
            case CMD_KEY_RELEASE:
            case CMD_KEY_RELEASE_ALL:
            case CMD_KEY_TOGGLE:

                main.keyboard.processCommand(cmd);
                break;

            case CMD_DSK_TOGGLE_WP:

                switch (cmd.value) {

                    case DRIVE8: main.drive8.processCommand(cmd); break;
                    case DRIVE9: main.drive9.processCommand(cmd); break;
                    default: fatalError;
                }
                break;

            case CMD_MOUSE_MOVE_ABS:
            case CMD_MOUSE_MOVE_REL:

                switch (cmd.coord.port) {

                    case PORT_1: main.port1.processCommand(cmd); break;
                    case PORT_2: main.port2.processCommand(cmd); break;
                    default: fatalError;
                }
                break;

            case CMD_MOUSE_EVENT:
            case CMD_JOY_EVENT:

                switch (cmd.action.port) {

                    case PORT_1: main.port1.processCommand(cmd); break;
                    case PORT_2: main.port2.processCommand(cmd); break;
                    default: fatalError;
                }
                break;

            case CMD_DATASETTE_PLAY:
            case CMD_DATASETTE_STOP:
            case CMD_DATASETTE_REWIND:

                main.datasette.processCommand(cmd);
                break;

            case CMD_CRT_BUTTON_PRESS:
            case CMD_CRT_BUTTON_RELEASE:
            case CMD_CRT_SWITCH_LEFT:
            case CMD_CRT_SWITCH_NEUTRAL:
            case CMD_CRT_SWITCH_RIGHT:

                main.expansionport.processCommand(cmd);
                break;

            case CMD_RSH_EXECUTE:
                
                main.retroShell.exec();
                break;

            default:
                fatal("Unhandled command: %s\n", CmdTypeEnum::key(cmd.type));
        }
    }
}

void
Emulator::computeFrame()
{
    if (config.runAhead) {

        // Run the main instance
        main.executeHeadless();

        // Recreate the run-ahead instance if necessary
        if (main.isDirty || RUA_ON_STEROIDS) recreateRunAheadInstance();

        // Run the runahead instance
        ahead.execute();

    } else {

        // Run the main instance
        main.execute();
    }
}

void 
Emulator::recreateRunAheadInstance()
{
    debug(RUA_DEBUG, "%lld: Recomputing the run-ahead instance\n", main.frame);

    // Recreate the runahead instance from scratch
    ahead = main; main.isDirty = false;

    if (RUA_DEBUG && ahead != main) {

        main.dump(Category::Checksums);
        ahead.dump(Category::Checksums);
        fatal("Corrupted run-ahead clone detected");
    }

    // Advance to the proper frame
    ahead.fastForward(config.runAhead - 1);
}

u32 *
Emulator::getTexture() const
{
    // Return a noise pattern if the emulator is powered off
    if (isPoweredOff()) return main.vic.getNoise();

    // Get the texture from the proper emulator instance
    return config.runAhead ? ahead.vic.getTexture() : main.vic.getTexture();
}

u32 *
Emulator::getNoise() const
{
    return main.vic.getNoise();
}

void
Emulator::put(const Cmd &cmd)
{
    cmdQueue.put(cmd);
    
    if (cmd.type == CMD_HALT) {
        join();
    }
}

void
Emulator::process(const Cmd &cmd)
{

}

double
Emulator::refreshRate() const
{
    if (config.vsync) {

        return double(host.getOption(OPT_HOST_REFRESH_RATE));

    } else {

        return main.vic.getFps() * config.timeLapse / 100.0;
    }
}

}
