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
    halt();
}

void
Emulator::launch(const void *listener, Callback *func)
{
    // Initialize the emulator if needed
    if (!isInitialized()) initialize();

    // Connect the listener to the message queue of the main instance
    main.msgQueue.setListener(listener, func);

    // Disable the message queue of the run-ahead instance
    ahead.msgQueue.disable();

    // Launch the emulator thread
    Thread::launch();
}

void
Emulator::initialize()
{
    // Make sure this function is only called once
    if (isInitialized()) throw Error(VC64ERROR_LAUNCH, "The emulator is already initialized.");

    // Initialize all components
    main.initialize();
    ahead.initialize();

    // Setup the default configuration
    revertToFactorySettings();

    // Switch state
    state = newState = STATE_OFF;
    assert(isInitialized());
}

void
Emulator::hardReset()
{
    {   SUSPENDED

        main.hardReset();
    }
}

void
Emulator::softReset()
{
    {   SUSPENDED

        main.softReset();
    }
}

void
Emulator::stepInto()
{
    if (isRunning()) return;

    main.stepTo = { };
    main.setFlag(RL::SINGLE_STEP);
    run();
}

void 
Emulator::stepOver()
{
    if (isRunning()) return;

    main.stepTo = main.cpu.getAddressOfNextInstruction();
    main.setFlag(RL::SINGLE_STEP);
    run();
}

void
Emulator::revertToFactorySettings()
{
    // Setup the default configuration
    main.resetConfig();
    ahead.resetConfig();

    // Perform a hard reset
    main.hardReset();
    ahead.hardReset();
}

u32 *
Emulator::getTexture() const
{
    return main.config.runAhead && isRunning() ?
    ahead.videoPort.getTexture() :
    main.videoPort.getTexture();
}

u32 *
Emulator::getDmaTexture() const
{
    return main.config.runAhead && isRunning() ?
    ahead.videoPort.getDmaTexture() :
    main.videoPort.getDmaTexture();
}

void
Emulator::put(const Cmd &cmd)
{
    cmdQueue.put(cmd);
}

/*
void
Emulator::process(const Cmd &cmd)
{

}
*/

i64
Emulator::get(Option opt, isize objid) const
{
    return main.get(opt, objid);
}

void
Emulator::check(Option opt, i64 value, const std::vector<isize> objids)
{
    return main.check(opt, value, objids);
}

void
Emulator::set(Option opt, i64 value, const std::vector<isize> objids)
{
    return main.set(opt, value, objids);
}

void
Emulator::set(Option opt, const string &value, const std::vector<isize> objids)
{
    return main.set(opt, value, objids);
}

void
Emulator::set(const string &opt, const string &value, const std::vector<isize> objids)
{
    return main.set(opt, value, objids);
}

void
Emulator::set(C64Model model)
{
    main.set(model);
}

void
Emulator::update()
{
    // Switch warp mode on or off
    shouldWarp() ? warpOn() : warpOff();

    // Mark the run-ahead instance dirty when the command queue has entries
    isDirty |= !cmdQueue.empty;

    // Process all commands
    main.update(cmdQueue);
}

bool
Emulator::shouldWarp()
{
    auto &config = main.getConfig();

    if (main.cpu.clock < C64::sec(config.warpBoot)) {

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
    auto &config = main.getConfig();

    // In VSYNC mode, compute exactly one frame per wakeup call
    if (config.vsync) return 1;

    // Compute the elapsed time
    auto elapsed = util::Time::now() - baseTime;

    // Compute which slice should be reached by now
    auto target = elapsed.asNanoseconds() * i64(refreshRate()) / 1000000000;

    // Compute the number of missing slices
    return isize(target - frameCounter);
}

double
Emulator::refreshRate() const
{
    auto &config = main.getConfig();

    if (config.vsync) {

        return double(main.host.getOption(OPT_HOST_REFRESH_RATE));

    } else {

        return main.vic.getFps() * config.speedAdjust / 100.0;
    }
}

void
Emulator::computeFrame()
{
    auto &config = main.getConfig();

    if (config.runAhead) {

        try {

            // Run the main instance
            main.computeFrame();

            // Recreate the run-ahead instance if necessary
            if (isDirty || RUA_ON_STEROIDS) recreateRunAheadInstance();

            // Run the runahead instance
            ahead.computeFrame();

        } catch (StateChangeException &) {

            isDirty = true;
            throw;
        }

    } else {

        // Only run the main instance
        main.computeFrame();
    }
}

void
Emulator::cloneRunAheadInstance()
{
    clones++;

    // Recreate the runahead instance from scratch
    ahead = main; isDirty = false;

    if (RUA_CHECKSUM && ahead != main) {

        main.diff(ahead);
        fatal("Corrupted run-ahead clone detected");
    }
}

void
Emulator::recreateRunAheadInstance()
{
    auto &config = main.getConfig();

    // Clone the main instance
    if (RUA_DEBUG) {
        util::StopWatch watch("Run-ahead: Clone");
        cloneRunAheadInstance();
    } else {
        cloneRunAheadInstance();
    }

    // Advance to the proper frame
    if (RUA_DEBUG) {
        util::StopWatch watch("Run-ahead: Fast-forward");
        ahead.fastForward(config.runAhead - 1);
    } else {
        ahead.fastForward(config.runAhead - 1);
    }
}

void
Emulator::isReady()
{
    main.isReady();
}

int
Emulator::getDebugVariable(DebugFlag flag)
{
#ifdef RELEASEBUILD

    throw Error(VC64ERROR_OPT_UNSUPPORTED, "Debug variables are only accessible in debug builds.");

#else

    switch (flag) {

        case FLAG_XFILES:           return XFILES;
        case FLAG_CNF_DEBUG:        return CNF_DEBUG;
        case FLAG_DEF_DEBUG:        return DEF_DEBUG;

        case FLAG_RUN_DEBUG:        return RUN_DEBUG;
        case FLAG_TIM_DEBUG:        return TIM_DEBUG;
        case FLAG_WARP_DEBUG:       return WARP_DEBUG;
        case FLAG_CMD_DEBUG:        return CMD_DEBUG;
        case FLAG_MSG_DEBUG:        return MSG_DEBUG;
        case FLAG_SNP_DEBUG:        return SNP_DEBUG;

        case FLAG_RUA_DEBUG:        return RUA_DEBUG;
        case FLAG_RUA_CHECKSUM:     return RUA_CHECKSUM;
        case FLAG_RUA_ON_STEROIDS:  return RUA_ON_STEROIDS;

        case FLAG_CPU_DEBUG:        return CPU_DEBUG;
        case FLAG_IRQ_DEBUG:        return IRQ_DEBUG;

        case FLAG_MEM_DEBUG:        return MEM_DEBUG;

        case FLAG_CIA_DEBUG:        return CIA_DEBUG;
        case FLAG_CIAREG_DEBUG:     return CIAREG_DEBUG;

        case FLAG_VICII_DEBUG:      return VICII_DEBUG;
        case FLAG_VICII_REG_DEBUG:  return VICII_REG_DEBUG;
        case FLAG_VICII_SAFE_MODE:  return VICII_SAFE_MODE;
        case FLAG_VICII_STATS:      return VICII_STATS;
        case FLAG_RASTERIRQ_DEBUG:  return RASTERIRQ_DEBUG;

        case FLAG_SID_DEBUG:        return SID_DEBUG;
        case FLAG_SID_EXEC:         return SID_EXEC;
        case FLAG_SIDREG_DEBUG:     return SIDREG_DEBUG;
        case FLAG_AUDBUF_DEBUG:     return AUDBUF_DEBUG;
        case FLAG_AUDVOL_DEBUG:     return AUDVOL_DEBUG;

        case FLAG_VIA_DEBUG:        return VIA_DEBUG;
        case FLAG_PIA_DEBUG:        return PIA_DEBUG;
        case FLAG_SER_DEBUG:        return SER_DEBUG;
        case FLAG_DSK_DEBUG:        return DSK_DEBUG;
        case FLAG_DSKCHG_DEBUG:     return DSKCHG_DEBUG;
        case FLAG_GCR_DEBUG:        return GCR_DEBUG;
        case FLAG_FS_DEBUG:         return FS_DEBUG;
        case FLAG_PAR_DEBUG:        return PAR_DEBUG;

        case FLAG_CRT_DEBUG:        return CRT_DEBUG;
        case FLAG_FILE_DEBUG:       return FILE_DEBUG;

        case FLAG_JOY_DEBUG:        return JOY_DEBUG;
        case FLAG_DRV_DEBUG:        return DRV_DEBUG;
        case FLAG_TAP_DEBUG:        return TAP_DEBUG;
        case FLAG_KBD_DEBUG:        return KBD_DEBUG;
        case FLAG_PRT_DEBUG:        return PRT_DEBUG;
        case FLAG_EXP_DEBUG:        return EXP_DEBUG;
        case FLAG_USR_DEBUG:        return USR_DEBUG;

        case FLAG_REC_DEBUG:        return REC_DEBUG;
        case FLAG_REU_DEBUG:        return REU_DEBUG;
        case FLAG_SCK_DEBUG:        return SCK_DEBUG;
        case FLAG_SRV_DEBUG:        return SRV_DEBUG;

        case FLAG_FORCE_ROM_MISSING:        return FORCE_ROM_MISSING;
        case FLAG_FORCE_MEGA64_MISMATCH:    return FORCE_MEGA64_MISMATCH;
        case FLAG_FORCE_SNAP_TOO_OLD:       return FORCE_SNAP_TOO_OLD;
        case FLAG_FORCE_SNAP_TOO_NEW:       return FORCE_SNAP_TOO_NEW;
        case FLAG_FORCE_SNAP_IS_BETA:       return FORCE_SNAP_IS_BETA;
        case FLAG_FORCE_SNAP_CORRUPTED:     return FORCE_SNAP_CORRUPTED;
        case FLAG_FORCE_CRT_UNKNOWN:        return FORCE_CRT_UNKNOWN;
        case FLAG_FORCE_CRT_UNSUPPORTED:    return FORCE_CRT_UNSUPPORTED;
        case FLAG_FORCE_RECORDING_ERROR:    return FORCE_RECORDING_ERROR;
        case FLAG_FORCE_NO_FFMPEG:          return FORCE_NO_FFMPEG;

        default:
            throw Error(VC64ERROR_OPT_UNSUPPORTED,
                        "Unhandled debug variable: " + string(DebugFlagEnum::key(flag)));
    }

#endif
}

void
Emulator::setDebugVariable(DebugFlag flag, bool val)
{
#ifdef RELEASEBUILD

    throw Error(VC64ERROR_OPT_UNSUPPORTED, "Debug variables are only accessible in debug builds.");

#else

    switch (flag) {

        case FLAG_XFILES:           XFILES          = val; break;
        case FLAG_CNF_DEBUG:        CNF_DEBUG       = val; break;
        case FLAG_DEF_DEBUG:        DEF_DEBUG       = val; break;

        case FLAG_RUN_DEBUG:        RUN_DEBUG       = val; break;
        case FLAG_TIM_DEBUG:        TIM_DEBUG       = val; break;
        case FLAG_WARP_DEBUG:       WARP_DEBUG      = val; break;
        case FLAG_CMD_DEBUG:        CMD_DEBUG       = val; break;
        case FLAG_MSG_DEBUG:        MSG_DEBUG       = val; break;
        case FLAG_SNP_DEBUG:        SNP_DEBUG       = val; break;

        case FLAG_RUA_DEBUG:        RUA_DEBUG       = val; break;
        case FLAG_RUA_CHECKSUM:     RUA_CHECKSUM    = val; break;
        case FLAG_RUA_ON_STEROIDS:  RUA_ON_STEROIDS = val; break;

        case FLAG_CPU_DEBUG:        CPU_DEBUG       = val; break;
        case FLAG_IRQ_DEBUG:        IRQ_DEBUG       = val; break;

        case FLAG_MEM_DEBUG:        MEM_DEBUG       = val; break;

        case FLAG_CIA_DEBUG:        CIA_DEBUG       = val; break;
        case FLAG_CIAREG_DEBUG:     CIAREG_DEBUG    = val; break;

        case FLAG_VICII_DEBUG:      VICII_DEBUG     = val; break;
        case FLAG_VICII_REG_DEBUG:  VICII_REG_DEBUG = val; break;
        case FLAG_VICII_SAFE_MODE:  VICII_SAFE_MODE = val; break;
        case FLAG_VICII_STATS:      VICII_STATS     = val; break;
        case FLAG_RASTERIRQ_DEBUG:  RASTERIRQ_DEBUG = val; break;

        case FLAG_SID_DEBUG:        SID_DEBUG       = val; break;
        case FLAG_SID_EXEC:         SID_EXEC        = val; break;
        case FLAG_SIDREG_DEBUG:     SIDREG_DEBUG    = val; break;
        case FLAG_AUDBUF_DEBUG:     AUDBUF_DEBUG    = val; break;
        case FLAG_AUDVOL_DEBUG:     AUDVOL_DEBUG    = val; break;

        case FLAG_VIA_DEBUG:        VIA_DEBUG       = val; break;
        case FLAG_PIA_DEBUG:        PIA_DEBUG       = val; break;
        case FLAG_SER_DEBUG:        SER_DEBUG       = val; break;
        case FLAG_DSK_DEBUG:        DSK_DEBUG       = val; break;
        case FLAG_DSKCHG_DEBUG:     DSKCHG_DEBUG    = val; break;
        case FLAG_GCR_DEBUG:        GCR_DEBUG       = val; break;
        case FLAG_FS_DEBUG:         FS_DEBUG        = val; break;
        case FLAG_PAR_DEBUG:        PAR_DEBUG       = val; break;

        case FLAG_CRT_DEBUG:        CRT_DEBUG       = val; break;
        case FLAG_FILE_DEBUG:       FILE_DEBUG      = val; break;

        case FLAG_JOY_DEBUG:        JOY_DEBUG       = val; break;
        case FLAG_DRV_DEBUG:        DRV_DEBUG       = val; break;
        case FLAG_TAP_DEBUG:        TAP_DEBUG       = val; break;
        case FLAG_KBD_DEBUG:        KBD_DEBUG       = val; break;
        case FLAG_PRT_DEBUG:        PRT_DEBUG       = val; break;
        case FLAG_EXP_DEBUG:        EXP_DEBUG       = val; break;
        case FLAG_USR_DEBUG:        USR_DEBUG       = val; break;

        case FLAG_REC_DEBUG:        REC_DEBUG       = val; break;
        case FLAG_REU_DEBUG:        REU_DEBUG       = val; break;
        case FLAG_SCK_DEBUG:        SCK_DEBUG       = val; break;
        case FLAG_SRV_DEBUG:        SRV_DEBUG       = val; break;

        case FLAG_FORCE_ROM_MISSING:        FORCE_ROM_MISSING = val; break;
        case FLAG_FORCE_MEGA64_MISMATCH:    FORCE_MEGA64_MISMATCH = val; break;
        case FLAG_FORCE_SNAP_TOO_OLD:       FORCE_SNAP_TOO_OLD = val; break;
        case FLAG_FORCE_SNAP_TOO_NEW:       FORCE_SNAP_TOO_NEW = val; break;
        case FLAG_FORCE_SNAP_IS_BETA:       FORCE_SNAP_IS_BETA = val; break;
        case FLAG_FORCE_SNAP_CORRUPTED:     FORCE_SNAP_CORRUPTED = val; break;
        case FLAG_FORCE_CRT_UNKNOWN:        FORCE_CRT_UNKNOWN = val; break;
        case FLAG_FORCE_CRT_UNSUPPORTED:    FORCE_CRT_UNSUPPORTED = val; break;
        case FLAG_FORCE_RECORDING_ERROR:    FORCE_RECORDING_ERROR = val; break;
        case FLAG_FORCE_NO_FFMPEG:          FORCE_NO_FFMPEG = val; break;

        default:
            throw Error(VC64ERROR_OPT_UNSUPPORTED,
                        "Unhandled debug variable: " + string(DebugFlagEnum::key(flag)));
    }
#endif
}

}
