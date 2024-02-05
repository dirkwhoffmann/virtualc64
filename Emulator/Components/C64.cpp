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
#include "C64.h"
#include "Checksum.h"
#include "IOUtils.h"
#include <algorithm>

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

Defaults C64::defaults;

string
C64::version()
{
    string result;

    result = std::to_string(VER_MAJOR) + "." + std::to_string(VER_MINOR);
    if constexpr (VER_SUBMINOR > 0) result += "." + std::to_string(VER_SUBMINOR);
    if constexpr (VER_BETA > 0) result += 'b' + std::to_string(VER_BETA);

    return result;
}

string
C64::build()
{
    string db = debugBuild ? " [DEBUG BUILD]" : "";

    return version() + db + " (" + __DATE__ + " " + __TIME__ + ")";
}

const char *
C64::eventName(EventSlot slot, EventID id)
{
    assert_enum(EventSlot, slot);

    switch (slot) {

        case SLOT_CIA1:
        case SLOT_CIA2:

            switch (id) {
                case EVENT_NONE:    return "none";
                case CIA_EXECUTE:   return "CIA_EXECUTE";
                case CIA_WAKEUP:    return "CIA_WAKEUP";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_SEC:

            switch (id) {

                case EVENT_NONE:    return "none";
                case SEC_TRIGGER:   return "SEC_TRIGGER";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_IEC:

            switch (id) {

                case EVENT_NONE:    return "none";
                case IEC_UPDATE:    return "IEC_UPDATE";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_DAT:

            switch (id) {

                case EVENT_NONE:    return "none";
                case DAT_EXECUTE:   return "DAT_EXECUTE";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_TER:

            switch (id) {

                case EVENT_NONE:    return "none";
                case TER_TRIGGER:   return "TER_TRIGGER";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_MOT:

            switch (id) {

                case EVENT_NONE:    return "none";
                case MOT_START:     return "MOT_START";
                case MOT_STOP:      return "MOT_STOP";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_DC8:
        case SLOT_DC9:

            switch (id) {

                case EVENT_NONE:    return "none";
                case DCH_INSERT:    return "DCH_INSERT";
                case DCH_EJECT:     return "DCH_EJECT";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_RSH:

            switch (id) {

                case EVENT_NONE:        return "none";
                case RSH_WAKEUP:        return "RSH_WAKEUP";
                default:                return "*** INVALID ***";
            }
            break;

        case SLOT_KEY:

            switch (id) {

                case EVENT_NONE:    return "none";
                case KEY_AUTO_TYPE: return "AUTO_TYPE";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_WBT:

            switch (id) {

                case EVENT_NONE:        return "none";
                case WBT_DISABLE:       return "WBT_DISABLE";
                default:                return "*** INVALID ***";
            }
            break;

        case SLOT_ALA:

            switch (id) {

                case EVENT_NONE:    return "none";
                case ALA_TRIGGER:   return "ALA_TRIGGER";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_INS:

            switch (id) {

                case EVENT_NONE:    return "none";
                case INS_C64:       return "INS_C64";
                case INS_CPU:       return "INS_CPU";
                case INS_MEM:       return "INS_MEM";
                case INS_CIA:       return "INS_CIA";
                case INS_VIC:       return "INS_VIC";
                case INS_SID:       return "INS_SID";
                case INS_EVENTS:    return "INS_EVENTS";
                default:            return "*** INVALID ***";
            }
            break;

        default:
            fatalError;
    }
}

C64::C64()
{
    trace(RUN_DEBUG, "Creating virtual C64\n");

    subComponents = std::vector<CoreComponent *> {
        
        &mem,
        &cpu,
        &cia1, &cia2,
        &vic,
        &muxer,
        &supply,
        &port1,
        &port2,
        &expansionport,
        &iec,
        &keyboard,
        &drive8,
        &drive9,
        &parCable,
        &datasette,
        &retroShell,
        &regressionTester,
        &recorder,
        &msgQueue
    };

    // Assign a unique ID to the CPU
    cpu.setID(0);

    // Set up the initial state
    CoreComponent::initialize();
    CoreComponent::reset(true);

    // Initialize the sync timer
    targetTime = util::Time::now();
}

C64::~C64()
{
    debug(RUN_DEBUG, "Destroying emulator instance\n");
    if (thread.joinable()) { halt(); }
}

void
C64::launch(const void *listener, Callback *func)
{
    msgQueue.setListener(listener, func);

    launch();
}

void
C64::launch()
{
    // Make sure to call this function only once
    assert(!thread.joinable());

    // Start the thread and enter the main function
    thread = std::thread(&Thread::main, this);
}

void
C64::prefix() const
{
    fprintf(stderr, "[%lld] (%3d,%3d) %04X ", frame, scanline, rasterCycle, cpu.getPC0());
}

void
C64::reset(bool hard)
{
    {   SUSPENDED
        
        // Execute the standard reset routine
        CoreComponent::reset(hard);

        // Reinitialize the program counter
        cpu.reg.pc = cpu.reg.pc0 = mem.resetVector();

        // Inform the GUI
        msgQueue.put(MSG_RESET);
    }
}

void
C64::_initialize()
{
    CoreComponent::_initialize();

    auto load = [&](const string &path) {

        msg("Trying to load Rom from %s...\n", path.c_str());

        try { loadRom(path); } catch (std::exception& e) {
            warn("Error: %s\n", e.what());
        }
    };

    if (auto path = C64::defaults.getString("BASIC_PATH");  path != "") load(path);
    if (auto path = C64::defaults.getString("CHAR_PATH");   path != "") load(path);
    if (auto path = C64::defaults.getString("KERNAL_PATH"); path != "") load(path);
    if (auto path = C64::defaults.getString("VC1541_PATH"); path != "") load(path);
}

void
C64::_reset(bool hard)
{
    auto insEvent = id[SLOT_INS];

    RESET_SNAPSHOT_ITEMS(hard)

    // Initialize all event slots
    for (isize i = 0; i < SLOT_COUNT; i++) {

        trigger[i] = NEVER;
        id[i] = (EventID)0;
        data[i] = 0;
    }

    // Schedule initial events
    scheduleAbs<SLOT_CIA1>(cpu.clock, CIA_EXECUTE);
    scheduleAbs<SLOT_CIA2>(cpu.clock, CIA_EXECUTE);
    scheduleAbs<SLOT_WBT>(SEC(config.warpBoot), WBT_DISABLE);
    if (insEvent) scheduleRel <SLOT_INS> (0, insEvent);

    flags = 0;
    rasterCycle = 1;
    updateWarpState();
}

void
C64::resetConfig()
{
    assert(isPoweredOff());

    std::vector <Option> options = {

        OPT_WARP_BOOT,
        OPT_WARP_MODE,
        OPT_SYNC_MODE,
        OPT_TIME_SLICES,
        OPT_AUTO_FPS,
        OPT_PROPOSED_FPS,
    };

    for (auto &option : options) {
        setConfigItem(option, defaults.get(option));
    }
}


i64
C64::getConfigItem(Option option) const
{
    switch (option) {

        case OPT_WARP_BOOT:

            return config.warpBoot;

        case OPT_WARP_MODE:

            return config.warpMode;

        case OPT_SYNC_MODE:

            return config.syncMode;

        case OPT_TIME_SLICES:

            return config.timeSlices;

        case OPT_AUTO_FPS:

            return config.autoFps;

        case OPT_PROPOSED_FPS:

            return config.proposedFps;

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
            return vic.getConfigItem(option);

        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_MODE:
        case OPT_DMA_DEBUG_OPACITY:
        case OPT_CUT_LAYERS:
        case OPT_CUT_OPACITY:
            return vic.dmaDebugger.getConfigItem(option);
            
        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:
            assert(cia1.getConfigItem(option) == cia2.getConfigItem(option));
            return cia1.getConfigItem(option);

        case OPT_POWER_GRID:
            return supply.getConfigItem(option);
            
        case OPT_SID_REVISION:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_FILTER:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_AUDVOLL:
        case OPT_AUDVOLR:
            return muxer.getConfigItem(option);

        case OPT_RAM_PATTERN:
        case OPT_SAVE_ROMS:
            return mem.getConfigItem(option);

        case OPT_DAT_MODEL:
        case OPT_DAT_CONNECT:
            return datasette.getConfigItem(option);
            
        default:
            fatalError;
    }
}

i64
C64::getConfigItem(Option option, long id) const
{
    const Drive &drive = id == DRIVE8 ? drive8 : drive9;
    
    switch (option) {
            
        case OPT_DMA_DEBUG_CHANNEL:
        case OPT_DMA_DEBUG_COLOR:
            
            return vic.dmaDebugger.getConfigItem(option, id);

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_AUDPAN:
        case OPT_AUDVOL:
            
            assert(id >= 0 && id <= 3);
            return muxer.getConfigItem(option, id);

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
            
            if (id == PORT_1) return port1.mouse.getConfigItem(option);
            if (id == PORT_2) return port2.mouse.getConfigItem(option);
            fatalError;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:
            
            if (id == PORT_1) return port1.joystick.getConfigItem(option);
            if (id == PORT_2) return port2.joystick.getConfigItem(option);
            fatalError;

        default:
            fatalError;
    }
}

void
C64::setConfigItem(Option option, i64 value)
{
    switch (option) {

        case OPT_WARP_BOOT:

            config.warpBoot = isize(value);
            updateWarpState();
            return;

        case OPT_WARP_MODE:

            if (!WarpModeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, WarpModeEnum::keyList());
            }

            config.warpMode = WarpMode(value);
            updateWarpState();
            return;

        case OPT_SYNC_MODE:

            if (!SyncModeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, SyncModeEnum::keyList());
            }

            config.syncMode = SyncMode(value);
            updateClockFrequency();
            return;

        case OPT_TIME_SLICES:

            if (value < 1 || value > 4) {
                throw VC64Error(ERROR_OPT_INVARG, "1...4");
            }

            config.timeSlices = isize(value);
            return;

        case OPT_AUTO_FPS:

            config.autoFps = bool(value);
            return;

        case OPT_PROPOSED_FPS:

            if (value < 25 || value > 120) {
                throw VC64Error(ERROR_OPT_INVARG, "25...120");
            }

            config.proposedFps = isize(value);
            updateClockFrequency();
            return;

        default:
            fatalError;
    }
}

void
C64::configure(Option option, i64 value)
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

        case OPT_WARP_BOOT:
        case OPT_WARP_MODE:
        case OPT_SYNC_MODE:
        case OPT_TIME_SLICES:
        case OPT_AUTO_FPS:
        case OPT_PROPOSED_FPS:

            setConfigItem(option, value);
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

            vic.setConfigItem(option, value);
            break;

        case OPT_CUT_LAYERS:
        case OPT_CUT_OPACITY:
        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_MODE:
        case OPT_DMA_DEBUG_OPACITY:

            vic.dmaDebugger.setConfigItem(option, value);
            break;

        case OPT_POWER_GRID:
            
            supply.setConfigItem(option, value);
            break;

        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:
            
            cia1.setConfigItem(option, value);
            cia2.setConfigItem(option, value);
            break;

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:

            muxer.setConfigItem(option, 0, value);
            muxer.setConfigItem(option, 1, value);
            muxer.setConfigItem(option, 2, value);
            muxer.setConfigItem(option, 3, value);

        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_SAMPLING:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_ENGINE:
        case OPT_AUDPAN:
        case OPT_AUDVOL:
        case OPT_AUDVOLL:
        case OPT_AUDVOLR:

            muxer.setConfigItem(option, value);
            break;
            
        case OPT_RAM_PATTERN:
        case OPT_SAVE_ROMS:

            mem.setConfigItem(option, value);
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
            
            drive8.setConfigItem(option, value);
            drive9.setConfigItem(option, value);
            break;

        case OPT_DAT_MODEL:
        case OPT_DAT_CONNECT:
            datasette.setConfigItem(option, value);
            
        case OPT_MOUSE_MODEL:
        case OPT_SHAKE_DETECTION:
        case OPT_MOUSE_VELOCITY:

            port1.mouse.setConfigItem(option, value);
            port2.mouse.setConfigItem(option, value);
            break;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            port1.joystick.setConfigItem(option, value);
            port2.joystick.setConfigItem(option, value);
            break;
        default:
            fatalError;
    }
    
    if (std::find(quiet.begin(), quiet.end(), option) == quiet.end()) {
        msgQueue.put(MSG_CONFIG, option);
    }
}

void
C64::configure(Option option, long id, i64 value)
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
            
            vic.dmaDebugger.setConfigItem(option, id, value);
            break;

        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:
            
            switch (id) {
                case 0: cia1.setConfigItem(option, value); break;
                case 1: cia2.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_MOUSE_MODEL:
        case OPT_SHAKE_DETECTION:
        case OPT_MOUSE_VELOCITY:

            switch (id) {
                case PORT_1: port1.mouse.setConfigItem(option, value); break;
                case PORT_2: port2.mouse.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            switch (id) {
                case PORT_1: port1.joystick.setConfigItem(option, value); break;
                case PORT_2: port2.joystick.setConfigItem(option, value); break;
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

            muxer.setConfigItem(option, id, value);
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
                case DRIVE8: drive8.setConfigItem(option, value); break;
                case DRIVE9: drive9.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;
            
        default:
            fatalError;
    }
    
    if (std::find(quiet.begin(), quiet.end(), option) == quiet.end()) {
        msgQueue.put(MSG_CONFIG, option);
    }
}

void
C64::configure(C64Model model)
{
    assert_enum(C64Model, model);
    
    {   SUSPENDED
        
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

EventSlotInfo
C64::getSlotInfo(isize nr) const
{
    assert_enum(EventSlot, nr);

    {   SYNCHRONIZED

        // if (!isRunning()) inspectSlot(nr);
        inspectSlot(nr);
        return slotInfo[nr];
    }
}

void
C64::inspectSlot(EventSlot nr) const
{
    assert_enum(EventSlot, nr);

    auto &info = slotInfo[nr];
    auto cycle = trigger[nr];

    info.slot = nr;
    info.eventId = id[nr];
    info.trigger = cycle;
    info.triggerRel = cycle - cpu.clock;

    // auto beam = pos + isize(AS_DMA_CYCLES(cycle - clock));

    // Compute clock at pos (0,0)
    auto clock00 = cpu.clock - vic.getCyclesPerLine() * scanline - rasterCycle;

    // Compute the number of elapsed cycles since then
    auto diff = cycle - clock00;

    // Split into frame / line / cycle
    info.frameRel = long(diff / vic.getCyclesPerFrame());
    diff = diff % vic.getCyclesPerFrame();
    info.vpos = long(diff / vic.getCyclesPerLine());
    info.hpos = long(diff % vic.getCyclesPerLine());

    info.eventName = eventName((EventSlot)nr, id[nr]);
}

void
C64::revertToFactorySettings()
{
    // Power off the emulator
    powerOff();

    // Put all components into their initial state
    initialize();
}

void
C64::updateClockFrequency()
{
    auto nativeFps = vic.getFps();
    auto chosenFps = refreshRate();

    auto nativeFrequency = vic.getFrequency();
    auto chosenFrequency = nativeFrequency * chosenFps / nativeFps;

    muxer.setClockFrequency((u32)chosenFrequency);
    durationOfOneCycle = 10000000000 / vic.getFrequency();
}

i64
C64::overrideOption(Option option, i64 value)
{
    static std::map<Option,i64> overrides = OVERRIDES;

    if (overrides.find(option) != overrides.end()) {

        msg("Overriding option: %s = %lld\n", OptionEnum::key(option), value);
        return overrides[option];
    }

    return value;
}

InspectionTarget
C64::getInspectionTarget() const
{
    switch(id[SLOT_INS]) {

        case EVENT_NONE:  return INSPECTION_NONE;
        case INS_C64:     return INSPECTION_C64;
        case INS_CPU:     return INSPECTION_CPU;
        case INS_MEM:     return INSPECTION_MEM;
        case INS_CIA:     return INSPECTION_CIA;
        case INS_VIC:     return INSPECTION_VIC;
        case INS_SID:     return INSPECTION_SID;
        case INS_EVENTS:  return INSPECTION_EVENTS;

        default:
            fatalError;
    }
}


void
C64::setInspectionTarget(InspectionTarget target, Cycle trigger)
{
    EventID id;

    {   SUSPENDED

        switch(target) {

            case INSPECTION_NONE:    cancel<SLOT_INS>(); return;

            case INSPECTION_C64:     id = INS_C64; break;
            case INSPECTION_CPU:     id = INS_CPU; break;
            case INSPECTION_MEM:     id = INS_MEM; break;
            case INSPECTION_CIA:     id = INS_CIA; break;
            case INSPECTION_VIC:     id = INS_VIC; break;
            case INSPECTION_SID:     id = INS_SID; break;
            case INSPECTION_EVENTS:  id = INS_EVENTS; break;

            default:
                fatalError;
        }

        scheduleRel<SLOT_INS>(trigger, id);
        if (trigger == 0) processINSEvent(id);
    }
}

SyncMode
C64::getSyncMode() const
{
    return config.syncMode;
}

void
C64::execute()
{
    cpu.debugger.watchpointPC = -1;
    cpu.debugger.breakpointPC = -1;

    switch ((drive8.needsEmulation ? 2 : 0) + (drive9.needsEmulation ? 1 : 0)) {

        case 0b00: execute <false,false> (); break;
        case 0b01: execute <false,true>  (); break;
        case 0b10: execute <true,false>  (); break;
        case 0b11: execute <true,true>   (); break;

        default:
            fatalError;
    }
}

template <bool enable8, bool enable9> void
C64::execute()
{
    bool exit = false;
    auto lastCycle = vic.getCyclesPerLine();
    auto syncLine = nextSyncLine(scanline);

    do {

        //
        // Run the emulator for the (rest of the) current scanline
        //

        for (; rasterCycle <= lastCycle; rasterCycle++) {

            //
            // Run the emulator for one cycle
            //
            
            //  <---------- o2 low phase ----------->|<- o2 high phase ->|
            //                                       |                   |
            // ,-- C64 ------------------------------|-------------------|--,
            // |   ,-----,     ,-----,     ,-----,   |    ,-----,        |  |
            // |   |     |     |     |     |     |   |    |     |        |  |
            // '-->| CIA | --> | CIA | --> | VIC | --|--> | CPU | -------|--'
            //     |  1  |     |  2  |     |     |   |    |     |        |
            //     '-----'     '-----'     '-----'   |    '-----'        |
            //                                       |                   |
            //                                       |    ,--------,     |
            //                                       |    |        |     |
            // ,-- Drive ----------------------------|--> | VC1541 | ----|--,
            // |                                     |    |        |     |  |
            // |                                     |    '--------'     |  |
            // '-------------------------------------|-------------------|--'

            Cycle cycle = ++cpu.clock;

            //
            // First clock phase (o2 low)
            //

            if (nextTrigger <= cycle) processEvents(cycle);
            (vic.*vic.vicfunc[rasterCycle])();


            //
            // Second clock phase (o2 high)
            //

            cpu.execute<MOS_6510>();
            if constexpr (enable8) { drive8.execute(durationOfOneCycle); }
            if constexpr (enable9) { drive9.execute(durationOfOneCycle); }


            //
            // Process run loop flags
            //

            if (flags && processFlags()) { rasterCycle++; exit = true; break; }
        }

        // Finish the current scanline if we are at the end
        if (rasterCycle > lastCycle) endScanline();

        // Check if we have reached the next sync point
        if (scanline == syncLine) exit = true;

    } while (!exit);
    
    trace(TIM_DEBUG, "Syncing at scanline %d\n", scanline);
}

isize 
C64::nextSyncLine(isize line)
{
    switch (slicesPerFrame()) {

        case 2: return line < 156 ? 156 : 0;
        case 3: return line < 104 ? 104 : line < 208 ? 208 : 0;
        case 4: return line <  78 ?  78 : line < 156 ? 156 : line < 234 ? 234 : 0;

        default:
            return 0;
    }
}

bool
C64::processFlags()
{
    // The following flags will terminate the loop
    bool exit = flags & (RL::BREAKPOINT |
                         RL::WATCHPOINT |
                         RL::STOP |
                         RL::CPU_JAM |
                         RL::SINGLE_STEP);

    // Are we requested to take an auto-snapshot?
    if (flags & RL::AUTO_SNAPSHOT) {
        clearFlag(RL::AUTO_SNAPSHOT);
        autoSnapshot = new Snapshot(*this);
        msgQueue.put(MSG_AUTO_SNAPSHOT_TAKEN);
    }

    // Are we requested to take a user-snapshot?
    if (flags & RL::USER_SNAPSHOT) {
        clearFlag(RL::USER_SNAPSHOT);
        userSnapshot = new Snapshot(*this);
        msgQueue.put(MSG_USER_SNAPSHOT_TAKEN);
    }

    // Did we reach a breakpoint?
    if (flags & RL::BREAKPOINT) {
        clearFlag(RL::BREAKPOINT);
        msgQueue.put(MSG_BREAKPOINT_REACHED, CpuMsg {u16(cpu.debugger.breakpointPC)});
        inspect();
        switchState(EXEC_PAUSED);
    }

    // Did we reach a watchpoint?
    if (flags & RL::WATCHPOINT) {
        clearFlag(RL::WATCHPOINT);
        msgQueue.put(MSG_WATCHPOINT_REACHED, CpuMsg {u16(cpu.debugger.watchpointPC)});
        inspect();
        switchState(EXEC_PAUSED);
    }

    // Are we requested to terminate the run loop?
    if (flags & RL::STOP) {
        clearFlag(RL::STOP);
        switchState(EXEC_PAUSED);
    }

    // Are we requested to pull the NMI line down?
    if (flags & RL::EXTERNAL_NMI) {
        clearFlag(RL::EXTERNAL_NMI);
        cpu.pullDownNmiLine(INTSRC_EXP);
    }

    // Is the CPU jammed due the execution of an illegal instruction?
    if (flags & RL::CPU_JAM) {
        clearFlag(RL::CPU_JAM);
        msgQueue.put(MSG_CPU_JAMMED);
        switchState(EXEC_PAUSED);
    }

    // Are we requested to simulate a BRK instruction
    if (flags & RL::EXTERNAL_BRK) {
        clearFlag(RL::EXTERNAL_BRK);
        cpu.next = BRK;
        cpu.reg.pc0 = cpu.reg.pc - 1;
    }

    // Are we requested to run for a single cycle?
    if (flags & RL::SINGLE_STEP) {
        clearFlag(RL::SINGLE_STEP);
    }

    assert(flags == 0);
    return exit;
}

double
C64::refreshRate() const
{
    switch (config.syncMode) {

        case SYNC_PULSED:

            return host.getHostRefreshRate();

        case SYNC_PERIODIC:
        case SYNC_ADAPTIVE:

            return config.autoFps ? vic.getFps() : config.proposedFps;

        default:
            fatalError;
    }
}

isize 
C64::slicesPerFrame() const
{
    return config.timeSlices;
}

util::Time 
C64::wakeupPeriod() const
{
    return util::Time(i64(1000000000.0 / host.getHostRefreshRate()));
}

void
C64::_isReady() const
{
    bool mega = hasMega65Rom(ROM_TYPE_BASIC) && hasMega65Rom(ROM_TYPE_KERNAL);
    
    if (!hasRom(ROM_TYPE_BASIC)) {
        throw VC64Error(ERROR_ROM_BASIC_MISSING);
    }
    if (!hasRom(ROM_TYPE_CHAR)) {
        throw VC64Error(ERROR_ROM_CHAR_MISSING);
    }
    if (!hasRom(ROM_TYPE_KERNAL) || FORCE_ROM_MISSING) {
        throw VC64Error(ERROR_ROM_KERNAL_MISSING);
    }
    if (FORCE_MEGA64_MISMATCH || (mega && string(mega65BasicRev()) != string(mega65KernalRev()))) {
        throw VC64Error(ERROR_ROM_MEGA65_MISMATCH);
    }
}

void
C64::_powerOn()
{
    debug(RUN_DEBUG, "_powerOn\n");
    
    // Perform a reset
    hardReset();

    // Update the recorded debug information
    inspect();

    msgQueue.put(MSG_POWER, 1);
}

void
C64::_powerOff()
{
    debug(RUN_DEBUG, "_powerOff\n");

    // Update the recorded debug information
    inspect();

    msgQueue.put(MSG_POWER, 0);
}

void
C64::_run()
{
    debug(RUN_DEBUG, "_run\n");

    msgQueue.put(MSG_RUN);
}

void
C64::_pause()
{
    debug(RUN_DEBUG, "_pause\n");

    // Finish the current instruction to reach a clean state
    finishInstruction();
    
    // Update the recorded debug information
    inspect();

    msgQueue.put(MSG_PAUSE);
}

void
C64::_halt()
{
    debug(RUN_DEBUG, "_halt\n");

    msgQueue.put(MSG_SHUTDOWN);
}

void
C64::_warpOn()
{
    debug(RUN_DEBUG, "_warpOn\n");

    msgQueue.put(MSG_WARP, 1);
}

void
C64::_warpOff()
{
    debug(RUN_DEBUG, "_warpOff\n");

    msgQueue.put(MSG_WARP, 0);
}

void
C64::_trackOn()
{
    debug(RUN_DEBUG, "_trackOn\n");

    msgQueue.put(MSG_TRACK, 1);
}

void
C64::_trackOff()
{
    debug(RUN_DEBUG, "_trackOff\n");

    msgQueue.put(MSG_TRACK, 0);
}

void
C64::_inspect() const
{
    SYNCHRONIZED

    eventInfo.cpuProgress = cpu.clock;
    eventInfo.cia1Progress = cia1.sleeping ? cia1.sleepCycle : cpu.clock;
    eventInfo.cia2Progress = cia2.sleeping ? cia2.sleepCycle : cpu.clock;
    eventInfo.frame = frame;
    eventInfo.vpos = scanline;
    eventInfo.hpos = rasterCycle;

    for (EventSlot i = 0; i < SLOT_COUNT; i++) {
        inspectSlot(i);
    }
}

isize
C64::load(const u8 *buffer)
{
    auto result = CoreComponent::load(buffer);
    CoreComponent::didLoad();

    return result;
}

isize
C64::save(u8 *buffer)
{
    auto result = CoreComponent::save(buffer);
    CoreComponent::didSave();

    return result;
}

void
C64::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        os << tab("Warp mode");
        os << WarpModeEnum::key(config.warpMode) << std::endl;
        os << tab("Warp boot");
        os << dec(config.warpBoot) << " seconds" << std::endl;
        os << tab("Sync mode");
        os << SyncModeEnum::key(config.syncMode) << std::endl;
        os << tab("Time slices");
        os << config.timeSlices << std::endl;
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
        os << tab("Ultimax mode");
        os << bol(getUltimax()) << std::endl;
        os << std::endl;

        os << tab("Frame");
        os << dec(frame) << std::endl;
        os << tab("CPU progress");
        os << dec(cpu.clock) << " Cycles" << std::endl;
        os << tab("CIA 1 progress");
        os << dec(cia1.isSleeping() ? cia1.sleepCycle : cpu.clock) << " Cycles" << std::endl;
        os << tab("CIA 2 progress");
        os << dec(cia2.isSleeping() ? cia2.sleepCycle : cpu.clock) << " Cycles" << std::endl;
    }

    if (category == Category::Summary) {

        auto vicRev = (VICIIRevision)getConfigItem(OPT_VIC_REVISION);
        auto sidRev = (SIDRevision)getConfigItem(OPT_SID_REVISION);
        auto cia1Rev = (CIARevision)cia1.getConfigItem(OPT_CIA_REVISION);
        auto cia2Rev = (CIARevision)cia2.getConfigItem(OPT_CIA_REVISION);

        os << tab("Model");
        os << (vic.pal() ? "PAL" : "NTSC") << std::endl;
        os << tab("VICII");
        os << VICIIRevisionEnum::key(vicRev) << std::endl;
        os << tab("SID");
        os << SIDRevisionEnum::key(sidRev) << std::endl;
        os << tab("CIA 1");
        os << CIARevisionEnum::key(cia1Rev) << std::endl;
        os << tab("CIA 2");
        os << CIARevisionEnum::key(cia2Rev) << std::endl;
        os << tab("Refresh rate");
        os << dec(isize(refreshRate())) << " Fps" << std::endl;
    }

    if (category == Category::Defaults) {

        defaults.dump(category, os);
    }

    if (category == Category::Current) {

        os << std::setfill('0') << std::uppercase << std::hex << std::left;
        os << " PC  SR AC XR YR SP  NV-BDIZC" << std::endl;
        os << std::setw(4) << isize(cpu.reg.pc0) << " ";
        os << std::setw(2) << isize(cpu.getP()) << " ";
        os << std::setw(2) << isize(cpu.reg.a) << " ";
        os << std::setw(2) << isize(cpu.reg.x) << " ";
        os << std::setw(2) << isize(cpu.reg.y) << " ";
        os << std::setw(2) << isize(cpu.reg.sp) << "  ";
        os << (cpu.getN() ? "1" : "0");
        os << (cpu.getV() ? "1" : "0");
        os << "1";
        os << (cpu.getB() ? "1" : "0");
        os << (cpu.getD() ? "1" : "0");
        os << (cpu.getI() ? "1" : "0");
        os << (cpu.getZ() ? "1" : "0");
        os << (cpu.getC() ? "1" : "0");
        os << std::endl;
    }
}

void
C64::stopAndGo()
{
    isRunning() ? pause() : run();
}

void
C64::stepInto()
{
    if (isRunning()) return;
    
    // Execute the next instruction
    executeOneCycle();
    finishInstruction();
    
    // Inform the GUI
    msgQueue.put(MSG_STEP);
}

void
C64::stepOver()
{
    if (isRunning()) return;
    
    // If the next instruction is a JSR instruction (0x20), we set a breakpoint
    // at the next memory location. Otherwise, stepOver behaves like stepInto.
    if (mem.spypeek(cpu.getPC0()) == 0x20) {
        cpu.debugger.setSoftStopAtNextInstr();
        run();
    } else {
        stepInto();
    }
}

void
C64::executeOneCycle()
{
    setFlag(RL::SINGLE_STEP);
    execute();
    clearFlag(RL::SINGLE_STEP);
}

void
C64::finishInstruction()
{
    while (!cpu.inFetchPhase()) executeOneCycle();
}

void
C64::finishFrame()
{
    while (scanline != 0 || rasterCycle > 1) executeOneCycle();
}

void
C64::endScanline()
{
    cia1.tod.increment();
    cia2.tod.increment();

    vic.endScanline();
    rasterCycle = 1;
    scanline++;
    
    if (scanline >= vic.getLinesPerFrame()) {
        scanline = 0;
        endFrame();
    }
}

void
C64::endFrame()
{
    frame++;
    
    vic.endFrame();

    // Execute remaining SID cycles
    muxer.executeUntil(cpu.clock);
    
    // Execute other components
    iec.execute();
    expansionport.execute();
    port1.execute();
    port2.execute();
    drive8.vsyncHandler();
    drive9.vsyncHandler();
    recorder.vsyncHandler();
}

void
C64::processEvents(Cycle cycle)
{
    //
    // Check primary slots
    //

    if (isDue<SLOT_CIA1>(cycle)) {
        cia1.serviceEvent(id[SLOT_CIA1]);
    }
    if (isDue<SLOT_CIA2>(cycle)) {
        cia2.serviceEvent(id[SLOT_CIA2]);
    }

    if (isDue<SLOT_SEC>(cycle)) {

        //
        // Check secondary slots
        //

        if (isDue<SLOT_IEC>(cycle)) {
            iec.update();
        }

        if (isDue<SLOT_DAT>(cycle)) {
            datasette.processDatEvent(id[SLOT_DAT], data[SLOT_DAT]);
        }

        if (isDue<SLOT_TER>(cycle)) {

            //
            // Check tertiary slots
            //

            if (isDue<SLOT_MOT>(cycle)) {
                datasette.processMotEvent(id[SLOT_MOT]);
            }

            if (isDue<SLOT_DC8>(cycle)) {
                drive8.processDiskChangeEvent(id[SLOT_DC8]);
            }
            if (isDue<SLOT_DC9>(cycle)) {
                drive9.processDiskChangeEvent(id[SLOT_DC9]);
            }
            if (isDue<SLOT_RSH>(cycle)) {
                retroShell.serviceEvent();
            }
            if (isDue<SLOT_KEY>(cycle)) {
                keyboard.processKeyEvent(id[SLOT_KEY]);
            }
            if (isDue<SLOT_WBT>(cycle)) {
                processWBTEvent();
            }
            if (isDue<SLOT_ALA>(cycle)) {
                processAlarmEvent();
            }
            if (isDue<SLOT_INS>(cycle)) {

                processINSEvent(id[SLOT_INS]);
            }

            // Determine the next trigger cycle for all tertiary slots
            Cycle next = trigger[SLOT_TER + 1];
            for (isize i = SLOT_TER + 2; i < SLOT_COUNT; i++) {
                if (trigger[i] < next) next = trigger[i];
            }
            rescheduleAbs<SLOT_TER>(next);
        }

        // Determine the next trigger cycle for all secondary slots
        Cycle next = trigger[SLOT_SEC + 1];
        for (isize i = SLOT_SEC + 2; i <= SLOT_TER; i++) {
            if (trigger[i] < next) next = trigger[i];
        }
        rescheduleAbs<SLOT_SEC>(next);
    }

    // Determine the next trigger cycle for all primary slots
    Cycle next = trigger[0];
    for (isize i = 1; i <= SLOT_SEC; i++) {
        if (trigger[i] < next) next = trigger[i];
    }
    nextTrigger = next;
}

void
C64::processINSEvent(EventID id)
{
    switch (id) {

        case INS_C64:       inspect(); break;
        case INS_CPU:       cpu.inspect(); break;
        case INS_MEM:       mem.inspect(); break;
        case INS_CIA:       cia1.inspect(); cia2.inspect(); break;
        case INS_VIC:       vic.inspect(); break;
        case INS_SID:       muxer.inspect(); break;
        case INS_EVENTS:    _inspect(); break;

        default:
            fatalError;
    }

    // Reschedule event
    rescheduleRel<SLOT_INS>((Cycle)(inspectionInterval * PAL_CYCLES_PER_SECOND));
}

void
C64::updateWarpState()
{
    if (cpu.clock < SEC(config.warpBoot)) {

        switchWarp(true);
        return;
    }

    switch (config.warpMode) {

        case WARP_AUTO:     switchWarp(iec.isTransferring()); break;
        case WARP_NEVER:    switchWarp(false); break;
        case WARP_ALWAYS:   switchWarp(true); break;

        default:
            fatalError;
    }
}

void
C64::processWBTEvent()
{
    assert(id[SLOT_WBT] == WBT_DISABLE);

    updateWarpState();
    cancel <SLOT_WBT> ();
}

void
C64::setFlag(u32 flag)
{
    SYNCHRONIZED

    flags |= flag;
}

void
C64::clearFlag(u32 flag)
{
    SYNCHRONIZED

    flags &= ~flag;
}

void
C64::requestAutoSnapshot()
{
    if (!isRunning()) {
        
        // Take snapshot immediately
        // autoSnapshot = Snapshot::makeWithC64(this);
        autoSnapshot = new Snapshot(*this);
        msgQueue.put(MSG_AUTO_SNAPSHOT_TAKEN);
        
    } else {
        
        // Schedule the snapshot to be taken
        signalAutoSnapshot();
    }
}

void
C64::requestUserSnapshot()
{
    if (!isRunning()) {
        
        // Take snapshot immediately
        // userSnapshot = Snapshot::makeWithC64(this);
        userSnapshot = new Snapshot(*this);
        msgQueue.put(MSG_USER_SNAPSHOT_TAKEN);
        
    } else {
        
        // Schedule the snapshot to be taken
        signalUserSnapshot();
    }
}

Snapshot *
C64::latestAutoSnapshot()
{
    Snapshot *result = autoSnapshot;
    autoSnapshot = nullptr;
    return result;
}

Snapshot *
C64::latestUserSnapshot()
{
    Snapshot *result = userSnapshot;
    userSnapshot = nullptr;
    return result;
}

void
C64::loadSnapshot(const Snapshot &snapshot)
{
    {   SUSPENDED

        try {

            // Restore the saved state
            load(snapshot.getData());

            // Clear the keyboard matrix to avoid constantly pressed keys
            keyboard.releaseAll();

            // Print some debug info if requested
            if (SNP_DEBUG) dump(Category::State);

        } catch (VC64Error &error) {

            /* If we reach this point, the emulator has been put into an
             * inconsistent state due to corrupted snapshot data. We cannot
             * continue emulation, because it would likely crash the
             * application. Because we cannot revert to the old state either,
             * we perform a hard reset to eliminate the inconsistency.
             */
            hardReset();
            throw error;
        }
    }

    // Inform the GUI
    msgQueue.put(MSG_SNAPSHOT_RESTORED);
}

u32
C64::romCRC32(RomType type) const
{
    if (!hasRom(type)) return 0;
    
    switch (type) {
            
        case ROM_TYPE_BASIC:  return util::crc32(mem.rom + 0xA000, 0x2000);
        case ROM_TYPE_CHAR:   return util::crc32(mem.rom + 0xD000, 0x1000);
        case ROM_TYPE_KERNAL: return util::crc32(mem.rom + 0xE000, 0x2000);
        case ROM_TYPE_VC1541: return drive8.mem.romCRC32();

        default:
            fatalError;
    }
}

u64
C64::romFNV64(RomType type) const
{
    if (!hasRom(type)) return 0;
    
    switch (type) {
            
        case ROM_TYPE_BASIC:  return util::fnv64(mem.rom + 0xA000, 0x2000);
        case ROM_TYPE_CHAR:   return util::fnv64(mem.rom + 0xD000, 0x1000);
        case ROM_TYPE_KERNAL: return util::fnv64(mem.rom + 0xE000, 0x2000);
        case ROM_TYPE_VC1541: return drive8.mem.romFNV64();

        default:
            fatalError;
    }
}

RomIdentifier
C64::romIdentifier(RomType type) const
{
    return RomFile::identifier(romFNV64(type));
}

const string
C64::romTitle(RomType type) const
{
    RomIdentifier rev = romIdentifier(type);
    
    switch (type) {
            
        case ROM_TYPE_BASIC:

            if (hasMega65Rom(ROM_TYPE_BASIC)) return "M.E.G.A. C64 OpenROM";
            return rev == ROM_UNKNOWN ? "Unknown Basic Rom" : RomFile::title(rev);

        case ROM_TYPE_CHAR:

            if (hasMega65Rom(ROM_TYPE_CHAR)) return "M.E.G.A. C64 OpenROM";
            return rev == ROM_UNKNOWN ? "Unknown Character Rom" : RomFile::title(rev);

        case ROM_TYPE_KERNAL:

            if (hasMega65Rom(ROM_TYPE_KERNAL)) return "M.E.G.A. C64 OpenROM";
            return rev == ROM_UNKNOWN ? "Unknown Kernal Rom" : RomFile::title(rev);

        case ROM_TYPE_VC1541:

            return rev == ROM_UNKNOWN ? "Unknown Drive Firmware" : RomFile::title(rev);

        default:
            fatalError;
    }
}

const string
C64::romSubTitle(u64 fnv) const
{
    RomIdentifier rev = RomFile::identifier(fnv);
    
    if (rev != ROM_UNKNOWN) return RomFile::subTitle(rev);
    
    static char str[32];
    snprintf(str, sizeof(str), "FNV %llx", fnv);
    return str;
}

const string
C64::romSubTitle(RomType type) const
{
    switch (type) {
            
        case ROM_TYPE_BASIC:

            if (hasMega65Rom(ROM_TYPE_BASIC)) return "Free Basic Replacement";
            return romSubTitle(romFNV64(ROM_TYPE_BASIC));

        case ROM_TYPE_CHAR:

            if (hasMega65Rom(ROM_TYPE_CHAR)) return "Free Charset Replacement";
            return romSubTitle(romFNV64(ROM_TYPE_CHAR));

        case ROM_TYPE_KERNAL:

            if (hasMega65Rom(ROM_TYPE_KERNAL)) return "Free Kernal Replacement";
            return romSubTitle(romFNV64(ROM_TYPE_KERNAL));

        case ROM_TYPE_VC1541:

            return romSubTitle(romFNV64(ROM_TYPE_VC1541));

        default:
            fatalError;
    }
}

const string
C64::romRevision(RomType type) const
{
    switch (type) {

        case ROM_TYPE_BASIC:

            if (hasMega65Rom(ROM_TYPE_BASIC)) return mega65BasicRev();
            return RomFile::revision(romIdentifier(ROM_TYPE_BASIC));

        case ROM_TYPE_CHAR:

            return RomFile::revision(romIdentifier(ROM_TYPE_CHAR));

        case ROM_TYPE_KERNAL:

            if (hasMega65Rom(ROM_TYPE_KERNAL)) return mega65KernalRev();
            return RomFile::revision(romIdentifier(ROM_TYPE_KERNAL));

        case ROM_TYPE_VC1541:

            return RomFile::revision(romIdentifier(ROM_TYPE_VC1541));

        default:
            fatalError;
    }
}

bool
C64::hasRom(RomType type) const
{
    switch (type) {
            
        case ROM_TYPE_BASIC:

            return (mem.rom[0xA000] | mem.rom[0xA001]) != 0x00;

        case ROM_TYPE_CHAR:

            return (mem.rom[0xD000] | mem.rom[0xD001]) != 0x00;

        case ROM_TYPE_KERNAL:

            return (mem.rom[0xE000] | mem.rom[0xE001]) != 0x00;

        case ROM_TYPE_VC1541:

            assert(drive8.mem.hasRom() == drive9.mem.hasRom());
            return drive8.mem.hasRom();

        default:
            fatalError;
    }
}

bool
C64::hasMega65Rom(RomType type) const
{
    RomIdentifier id;
    
    switch (type) {
            
        case ROM_TYPE_BASIC:

            return mem.rom[0xBF52] == 'O' && mem.rom[0xBF53] == 'R';

        case ROM_TYPE_CHAR:

            id = romIdentifier(ROM_TYPE_CHAR);
            return id == CHAR_MEGA65 || id == CHAR_PXLFONT_V23;

        case ROM_TYPE_KERNAL:

            return mem.rom[0xE4B9] == 'O' && mem.rom[0xE4BA] == 'R';

        case ROM_TYPE_VC1541:

            return false;

        default:
            fatalError;
    }
}

const char *
C64::mega65BasicRev() const
{
    static char rev[17];
    rev[0] = 0;
    
    if (hasMega65Rom(ROM_TYPE_BASIC)) std::memcpy(rev, &mem.rom[0xBF55], 16);
    rev[16] = 0;
    
    return rev;
}

const char *
C64::mega65KernalRev() const
{
    static char rev[17];
    rev[0] = 0;
    
    if (hasMega65Rom(ROM_TYPE_KERNAL)) std::memcpy(rev, &mem.rom[0xE4BC], 16);
    rev[16] = 0;
    
    return rev;
}

void
C64::loadRom(const string &path)
{
    RomFile file(path);
    loadRom(file);
}

void
C64::loadRom(const RomFile &file)
{
    switch (file.type()) {
            
        case FILETYPE_BASIC_ROM:
            
            file.flash(mem.rom, 0xA000);
            debug(MEM_DEBUG, "Basic Rom flashed\n");
            debug(MEM_DEBUG, "hasMega65Rom() = %d\n", hasMega65Rom(ROM_TYPE_BASIC));
            debug(MEM_DEBUG, "mega65BasicRev() = %s\n", mega65BasicRev());
            break;
            
        case FILETYPE_CHAR_ROM:
            
            file.flash(mem.rom, 0xD000);
            debug(MEM_DEBUG, "Character Rom flashed\n");
            break;
            
        case FILETYPE_KERNAL_ROM:
            
            file.flash(mem.rom, 0xE000);
            debug(MEM_DEBUG, "Kernal Rom flashed\n");
            debug(MEM_DEBUG, "hasMega65Rom() = %d\n", hasMega65Rom(ROM_TYPE_KERNAL));
            debug(MEM_DEBUG, "mega65KernalRev() = %s\n", mega65KernalRev());
            break;
            
        case FILETYPE_VC1541_ROM:
            
            drive8.mem.loadRom(file.data, file.size);
            drive9.mem.loadRom(file.data, file.size);
            debug(MEM_DEBUG, "VC1541 Rom flashed\n");
            break;
            
        default:
            fatalError;
    }
}

void
C64::deleteRom(RomType type)
{
    switch (type) {
            
        case ROM_TYPE_BASIC:

            memset(mem.rom + 0xA000, 0, 0x2000);
            break;

        case ROM_TYPE_CHAR:

            memset(mem.rom + 0xD000, 0, 0x1000);
            break;

        case ROM_TYPE_KERNAL:

            memset(mem.rom + 0xE000, 0, 0x2000);
            break;

        case ROM_TYPE_VC1541:

            drive8.mem.deleteRom();
            drive9.mem.deleteRom();
            break;

        default:
            fatalError;
    }
}

void
C64::saveRom(RomType type, const string &path)
{
    switch (type) {
            
        case ROM_TYPE_BASIC:

            if (hasRom(ROM_TYPE_BASIC)) {
                RomFile file(mem.rom + 0xA000, 0x2000);
                file.writeToFile(path);
            }
            break;

        case ROM_TYPE_CHAR:

            if (hasRom(ROM_TYPE_CHAR)) {
                RomFile file(mem.rom + 0xD000, 0x1000);
                file.writeToFile(path);
            }
            break;

        case ROM_TYPE_KERNAL:

            if (hasRom(ROM_TYPE_KERNAL)) {
                RomFile file(mem.rom + 0xE000, 0x2000);
                file.writeToFile(path);
            }
            break;

        case ROM_TYPE_VC1541:

            if (hasRom(ROM_TYPE_VC1541)) {
                drive8.mem.saveRom(path);
            }
            break;
            
        default:
            fatalError;
    }
}

void
C64::flash(const AnyFile &file)
{
    {   SUSPENDED
        
        switch (file.type()) {
                
            case FILETYPE_BASIC_ROM:
                file.flash(mem.rom, 0xA000);
                break;
                
            case FILETYPE_CHAR_ROM:
                file.flash(mem.rom, 0xD000);
                break;
                
            case FILETYPE_KERNAL_ROM:
                file.flash(mem.rom, 0xE000);
                break;
                
            case FILETYPE_VC1541_ROM:
                drive8.mem.loadRom(dynamic_cast<const RomFile &>(file));
                drive9.mem.loadRom(dynamic_cast<const RomFile &>(file));
                break;

            case FILETYPE_SNAPSHOT:
                loadSnapshot(dynamic_cast<const Snapshot &>(file));
                break;
                
            default:
                fatalError;
        }
    }
}

void
C64::flash(const AnyCollection &file, isize nr)
{
    auto addr = (u16)file.itemLoadAddr(nr);
    auto size = file.itemSize(nr);
    if (size <= 2) return;
    
    {   SUSPENDED
        
        switch (file.type()) {
                
            case FILETYPE_D64:
            case FILETYPE_T64:
            case FILETYPE_P00:
            case FILETYPE_PRG:
            case FILETYPE_FOLDER:

                // Flash data into memory
                size = std::min(size - 2, isize(0x10000 - addr));
                file.copyItem(nr, mem.ram + addr, size, 2);

                // Rectify zero page
                mem.ram[0x2D] = LO_BYTE(addr + size);   // VARTAB (lo byte)
                mem.ram[0x2E] = HI_BYTE(addr + size);   // VARTAB (high byte)
                break;
                
            default:
                fatalError;
        }
    }
    
    msgQueue.put(MSG_FILE_FLASHED);
}

void
C64::flash(const FileSystem &fs, isize nr)
{
    u16 addr = fs.loadAddr(nr);
    u64 size = fs.fileSize(nr);
    
    if (size <= 2) {
        return;
    }
    
    {   SUSPENDED

        // Flash data into memory
        size = std::min(size - 2, (u64)(0x10000 - addr));
        fs.copyFile(nr, mem.ram + addr, size, 2);

        // Rectify zero page
        mem.ram[0x2D] = LO_BYTE(addr + size);   // VARTAB (lo byte)
        mem.ram[0x2E] = HI_BYTE(addr + size);   // VARTAB (high byte)
    }
    
    msgQueue.put(MSG_FILE_FLASHED);
}

void
C64::setAlarmAbs(Cycle trigger, i64 payload)
{
    {   SUSPENDED

        alarms.push_back(Alarm { trigger, payload });
        scheduleNextAlarm();
    }
}

void
C64::setAlarmRel(Cycle trigger, i64 payload)
{
    {   SUSPENDED

        alarms.push_back(Alarm { cpu.clock + trigger, payload });
        scheduleNextAlarm();
    }
}

void
C64::processAlarmEvent()
{
    for (auto it = alarms.begin(); it != alarms.end(); ) {

        if (it->trigger <= cpu.clock) {
            msgQueue.put(MSG_ALARM, it->payload);
            it = alarms.erase(it);
        } else {
            it++;
        }
    }
    scheduleNextAlarm();
}

void
C64::scheduleNextAlarm()
{
    Cycle trigger = INT64_MAX;

    cancel<SLOT_ALA>();

    for(Alarm alarm : alarms) {

        if (alarm.trigger < trigger) {
            scheduleAbs<SLOT_ALA>(alarm.trigger, ALA_TRIGGER);
            trigger = alarm.trigger;
        }
    }
}

fs::path
C64::tmp()
{
    STATIC_SYNCHRONIZED

    static fs::path base;

    if (base.empty()) {

        // Use /tmp as default folder for temporary files
        base = "/tmp";

        // Open a file to see if we have write permissions
        std::ofstream logfile(base / "virtualc64.log");

        // If /tmp is not accessible, use a different directory
        if (!logfile.is_open()) {

            base = fs::temp_directory_path();
            logfile.open(base / "virtualc64.log");

            if (!logfile.is_open()) {

                throw VC64Error(ERROR_DIR_NOT_FOUND);
            }
        }

        logfile.close();
        fs::remove(base / "virtualc64.log");
    }

    return base;
}

fs::path
C64::tmp(const string &name, bool unique)
{
    STATIC_SYNCHRONIZED

    auto base = tmp();
    auto result = base / name;

    // Make the file name unique if requested
    if (unique) result = fs::path(util::makeUniquePath(result.string()));

    return result;
}

void
C64::setDebugVariable(const string &name, int val)
{
#ifdef RELEASEBUILD

    throw VC64Error(ERROR_OPT_UNSUPPORTED, "Debug variables can only be altered in debug builds.");

#else

    if      (name == "XFILES")          XFILES          = val;
    else if (name == "CNF_DEBUG")       CNF_DEBUG       = val;
    else if (name == "DEF_DEBUG")       DEF_DEBUG       = val;

    else if (name == "RUN_DEBUG")       RUN_DEBUG       = val;
    else if (name == "TIM_DEBUG")       TIM_DEBUG       = val;
    else if (name == "WARP_DEBUG")      WARP_DEBUG      = val;
    else if (name == "MSG_DEBUG")       MSG_DEBUG       = val;
    else if (name == "SNP_DEBUG")       SNP_DEBUG       = val;

    else if (name == "CPU_DEBUG")       CPU_DEBUG       = val;
    else if (name == "IRQ_DEBUG")       IRQ_DEBUG       = val;

    else if (name == "MEM_DEBUG")       MEM_DEBUG       = val;

    else if (name == "CIA_DEBUG")       CIA_DEBUG       = val;
    else if (name == "CIAREG_DEBUG")    CIAREG_DEBUG    = val;
    else if (name == "CIA_ON_STEROIDS") CIA_ON_STEROIDS = val;


    else if (name == "VIC_DEBUG")       VIC_DEBUG       = val;
    else if (name == "VICREG_DEBUG")    VICREG_DEBUG    = val;
    else if (name == "RASTERIRQ_DEBUG") RASTERIRQ_DEBUG = val;
    else if (name == "VIC_SAFE_MODE")   VIC_SAFE_MODE   = val;
    else if (name == "VIC_STATS")       VIC_STATS       = val;

    else if (name == "SID_DEBUG")       SID_DEBUG       = val;
    else if (name == "SID_EXEC")        SID_EXEC        = val;
    else if (name == "SIDREG_DEBUG")    SIDREG_DEBUG    = val;
    else if (name == "AUDBUF_DEBUG")    AUDBUF_DEBUG    = val;

    // SID

    else if (name == "VIA_DEBUG")       VIA_DEBUG       = val;
    else if (name == "PIA_DEBUG")       PIA_DEBUG       = val;
    else if (name == "IEC_DEBUG")       IEC_DEBUG       = val;
    else if (name == "DSK_DEBUG")       DSK_DEBUG       = val;
    else if (name == "GCR_DEBUG")       GCR_DEBUG       = val;
    else if (name == "FS_DEBUG")        FS_DEBUG        = val;
    else if (name == "PAR_DEBUG")       PAR_DEBUG       = val;

    // Media
    else if (name == "CRT_DEBUG")       CRT_DEBUG       = val;
    else if (name == "FILE_DEBUG")      FILE_DEBUG      = val;


    else if (name == "JOY_DEBUG")       JOY_DEBUG       = val;
    else if (name == "DRV_DEBUG")       DRV_DEBUG       = val;
    else if (name == "TAP_DEBUG")       TAP_DEBUG       = val;
    else if (name == "KBD_DEBUG")       KBD_DEBUG       = val;
    else if (name == "PRT_DEBUG")       PRT_DEBUG       = val;
    else if (name == "EXP_DEBUG")       EXP_DEBUG       = val;
    else if (name == "LIP_DEBUG")       LIP_DEBUG       = val;

    else if (name == "REC_DEBUG")       REC_DEBUG       = val;
    else if (name == "REU_DEBUG")       REU_DEBUG       = val;

    else {

    throw VC64Error(ERROR_OPT_UNSUPPORTED, "Unknown debug variable: " + name);
}

#endif
}
}
