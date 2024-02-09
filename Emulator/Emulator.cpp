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

void
Emulator::configure(Option option, i64 value)
{
    debug(CNF_DEBUG, "configure(%ld, %lld)\n", option, value);

    // The following options do not send a message to the GUI
    static std::vector<Option> quiet = {

        OPT_BRIGHTNESS,
        OPT_CONTRAST,
        OPT_SATURATION,
        OPT_CUT_OPACITY,
        OPT_DMA_DEBUG_OPACITY,
        OPT_MOUSE_VELOCITY,
        OPT_AUTOFIRE_DELAY,
        OPT_AUDPAN,
        OPT_AUDVOL,
        OPT_AUDVOLL,
        OPT_AUDVOLR,
        OPT_DRV_PAN,
        OPT_DRV_POWER_VOL,
        OPT_DRV_STEP_VOL,
        OPT_DRV_INSERT_VOL,
        OPT_DRV_EJECT_VOL
    };

    // Check if this option has been locked for debugging
    value = overrideOption(option, value);

    switch (option) {

        case OPT_WARP_MODE:
        case OPT_WARP_BOOT:
        case OPT_VSYNC:
        case OPT_TIME_LAPSE:
        case OPT_RUN_AHEAD:

            setConfigItem(option, value);
            break;

        case OPT_HOST_REFRESH_RATE:
        case OPT_HOST_SAMPLE_RATE:
        case OPT_HOST_FRAMEBUF_WIDTH:
        case OPT_HOST_FRAMEBUF_HEIGHT:

            host.setConfigItem(option, value);
            break;

        case OPT_VIC_REVISION:
        case OPT_PALETTE:
        case OPT_BRIGHTNESS:
        case OPT_CONTRAST:
        case OPT_SATURATION:
        case OPT_GRAY_DOT_BUG:
        case OPT_VIC_POWER_SAVE:
        case OPT_HIDE_SPRITES:
        case OPT_SS_COLLISIONS:
        case OPT_SB_COLLISIONS:
        case OPT_GLUE_LOGIC:

            c64.vic.setConfigItem(option, value);
            break;

        case OPT_CUT_LAYERS:
        case OPT_CUT_OPACITY:
        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_MODE:
        case OPT_DMA_DEBUG_OPACITY:

            c64.vic.dmaDebugger.setConfigItem(option, value);
            break;

        case OPT_POWER_GRID:

            c64.supply.setConfigItem(option, value);
            break;

        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:

            c64.cia1.setConfigItem(option, value);
            c64.cia2.setConfigItem(option, value);
            break;

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:

            c64.muxer.setConfigItem(option, 0, value);
            c64.muxer.setConfigItem(option, 1, value);
            c64.muxer.setConfigItem(option, 2, value);
            c64.muxer.setConfigItem(option, 3, value);

        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_SAMPLING:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_ENGINE:
        case OPT_AUDPAN:
        case OPT_AUDVOL:
        case OPT_AUDVOLL:
        case OPT_AUDVOLR:

            c64.muxer.setConfigItem(option, value);
            break;

        case OPT_RAM_PATTERN:
        case OPT_SAVE_ROMS:

            c64.mem.setConfigItem(option, value);
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

            c64.drive8.setConfigItem(option, value);
            c64.drive9.setConfigItem(option, value);
            break;

        case OPT_DAT_MODEL:
        case OPT_DAT_CONNECT:
            c64.datasette.setConfigItem(option, value);

        case OPT_MOUSE_MODEL:
        case OPT_SHAKE_DETECTION:
        case OPT_MOUSE_VELOCITY:

            c64.port1.mouse.setConfigItem(option, value);
            c64.port2.mouse.setConfigItem(option, value);
            break;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            c64.port1.joystick.setConfigItem(option, value);
            c64.port2.joystick.setConfigItem(option, value);
            break;
        default:
            fatalError;
    }

    if (std::find(quiet.begin(), quiet.end(), option) == quiet.end()) {
        c64.msgQueue.put(MSG_CONFIG, option);
    }
}

void
Emulator::configure(Option option, long id, i64 value)
{
    debug(CNF_DEBUG, "configure(%ld, %ld, %lld)\n", option, id, value);

    // Check if this option has been locked for debugging
    value = overrideOption(option, value);

    // The following options do not send a message to the GUI
    static std::vector<Option> quiet = {

        OPT_MOUSE_VELOCITY,
        OPT_AUTOFIRE_DELAY,
        OPT_AUDPAN,
        OPT_AUDVOL,
        OPT_AUDVOLL,
        OPT_AUDVOLR,
        OPT_DRV_PAN,
        OPT_DRV_POWER_VOL,
        OPT_DRV_STEP_VOL,
        OPT_DRV_INSERT_VOL,
        OPT_DRV_EJECT_VOL
    };

    switch (option) {


        case OPT_DMA_DEBUG_CHANNEL:
        case OPT_DMA_DEBUG_COLOR:

            c64.vic.dmaDebugger.setConfigItem(option, id, value);
            break;

        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:

            switch (id) {
                case 0: c64.cia1.setConfigItem(option, value); break;
                case 1: c64.cia2.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_MOUSE_MODEL:
        case OPT_SHAKE_DETECTION:
        case OPT_MOUSE_VELOCITY:

            switch (id) {
                case PORT_1: c64.port1.mouse.setConfigItem(option, value); break;
                case PORT_2: c64.port2.mouse.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            switch (id) {
                case PORT_1: c64.port1.joystick.setConfigItem(option, value); break;
                case PORT_2: c64.port2.joystick.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_AUDPAN:
        case OPT_AUDVOL:
        case OPT_AUDVOLL:
        case OPT_AUDVOLR:

            c64.muxer.setConfigItem(option, id, value);
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
                case DRIVE8: c64.drive8.setConfigItem(option, value); break;
                case DRIVE9: c64.drive9.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;

        default:
            fatalError;
    }

    if (std::find(quiet.begin(), quiet.end(), option) == quiet.end()) {
        c64.msgQueue.put(MSG_CONFIG, option);
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

                configure(OPT_VIC_REVISION, VICII_PAL_6569_R3);
                configure(OPT_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_TIMER_B_BUG,  true);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_PAL_II:

                configure(OPT_VIC_REVISION, VICII_PAL_8565);
                configure(OPT_GRAY_DOT_BUG, true);
                configure(OPT_CIA_REVISION, MOS_8521);
                configure(OPT_TIMER_B_BUG,  false);
                configure(OPT_SID_REVISION, MOS_8580);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_PAL_OLD:

                configure(OPT_VIC_REVISION, VICII_PAL_6569_R1);
                configure(OPT_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_TIMER_B_BUG,  true);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC:

                configure(OPT_VIC_REVISION, VICII_NTSC_6567);
                configure(OPT_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_TIMER_B_BUG,  false);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC_II:

                configure(OPT_VIC_REVISION, VICII_NTSC_8562);
                configure(OPT_GRAY_DOT_BUG, true);
                configure(OPT_CIA_REVISION, MOS_8521);
                configure(OPT_TIMER_B_BUG,  true);
                configure(OPT_SID_REVISION, MOS_8580);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC_OLD:

                configure(OPT_VIC_REVISION, VICII_NTSC_6567_R56A);
                configure(OPT_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_TIMER_B_BUG,  false);
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
Emulator::getConfigItem(Option option) const
{
    switch (option) {

        case OPT_WARP_BOOT:     return config.warpBoot;
        case OPT_WARP_MODE:     return config.warpMode;
        case OPT_VSYNC:         return config.vsync;
        case OPT_TIME_LAPSE:    return config.timeLapse;
        case OPT_RUN_AHEAD:     return config.runAhead;

        case OPT_HOST_REFRESH_RATE:
        case OPT_HOST_SAMPLE_RATE:
        case OPT_HOST_FRAMEBUF_WIDTH:
        case OPT_HOST_FRAMEBUF_HEIGHT:

            return host.getConfigItem(option);

        case OPT_VIC_REVISION:
        case OPT_VIC_POWER_SAVE:
        case OPT_GRAY_DOT_BUG:
        case OPT_GLUE_LOGIC:
        case OPT_HIDE_SPRITES:
        case OPT_SS_COLLISIONS:
        case OPT_SB_COLLISIONS:

        case OPT_PALETTE:
        case OPT_BRIGHTNESS:
        case OPT_CONTRAST:
        case OPT_SATURATION:

            return c64.vic.getConfigItem(option);

        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_MODE:
        case OPT_DMA_DEBUG_OPACITY:
        case OPT_CUT_LAYERS:
        case OPT_CUT_OPACITY:

            return c64.vic.dmaDebugger.getConfigItem(option);

        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:

            assert(c64.cia1.getConfigItem(option) == c64.cia2.getConfigItem(option));
            return c64.cia1.getConfigItem(option);

        case OPT_POWER_GRID:

            return c64.supply.getConfigItem(option);

        case OPT_SID_REVISION:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_FILTER:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_AUDVOLL:
        case OPT_AUDVOLR:

            return c64.muxer.getConfigItem(option);

        case OPT_RAM_PATTERN:
        case OPT_SAVE_ROMS:

            return c64.mem.getConfigItem(option);

        case OPT_DAT_MODEL:
        case OPT_DAT_CONNECT:

            return c64.datasette.getConfigItem(option);

        default:
            fatalError;
    }
}

i64
Emulator::getConfigItem(Option option, long id) const
{
    const Drive &drive = id == DRIVE8 ? c64.drive8 : c64.drive9;

    switch (option) {

        case OPT_DMA_DEBUG_CHANNEL:
        case OPT_DMA_DEBUG_COLOR:

            return c64.vic.dmaDebugger.getConfigItem(option, id);

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_AUDPAN:
        case OPT_AUDVOL:

            assert(id >= 0 && id <= 3);
            return c64.muxer.getConfigItem(option, id);

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

            return drive.getConfigItem(option);

        case OPT_MOUSE_MODEL:
        case OPT_SHAKE_DETECTION:
        case OPT_MOUSE_VELOCITY:

            if (id == PORT_1) return c64.port1.mouse.getConfigItem(option);
            if (id == PORT_2) return c64.port2.mouse.getConfigItem(option);
            fatalError;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            if (id == PORT_1) return c64.port1.joystick.getConfigItem(option);
            if (id == PORT_2) return c64.port2.joystick.getConfigItem(option);
            fatalError;

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
            return;

        case OPT_WARP_MODE:

            if (!WarpModeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, WarpModeEnum::keyList());
            }

            config.warpMode = WarpMode(value);
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
    c64.isReady();
}

bool
Emulator::shouldWarp()
{
    if (c64.cpu.clock < SEC(config.warpBoot)) {

        return true;

    } else {

        switch (config.warpMode) {

            case WARP_AUTO:     return c64.iec.isTransferring();
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
    c64.execute();
}

double 
Emulator::refreshRate() const
{
    if (config.vsync) {

        return host.getHostRefreshRate();

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
