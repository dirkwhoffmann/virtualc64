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
static_assert(sizeof(i8)  == 1, "i8  size mismatch");
static_assert(sizeof(i16) == 2, "i16 size mismatch");
static_assert(sizeof(i32) == 4, "i32 size mismatch");
static_assert(sizeof(i64) == 8, "i64 size mismatch");
static_assert(sizeof(u8)  == 1, "u8  size mismatch");
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
    if (FORCE_LAUNCH_ERROR) throw CoreError(CoreError::LAUNCH);

    // Connect the listener to the message queue of the main instance
    if (listener && func) { main.msgQueue.setListener(listener, func); }

    // Disable the message queue of the run-ahead instance
    ahead.msgQueue.disable();

    // Launch the emulator thread
    Thread::launch();
}

void
Emulator::initialize()
{
    baseTime = utl::Time::now();

    // Make sure this function is only called once
    if (isInitialized()) throw CoreError(CoreError::LAUNCH, "The emulator is already initialized.");

    // Initialize all components
    main.initialize();
    ahead.initialize();

    // Setup the default configuration
    revertToDefaultConfig();

    // Get the runahead instance up-to-date
    ahead = main;

    // Switch state
    state = ExecState::OFF;

    // Mark the thread as initialized
    initLatch.count_down();
}

void
Emulator::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

    if (category == Category::Debug) {

        for (const auto &i : DebugFlagEnum::elements()) {

            os << tab(DebugFlagEnum::key(i));
            os << dec(getDebugVariable(DebugFlag(i))) << std::endl;
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
        os << dec(stats.clones) << std::endl;
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
    }
}

void
Emulator::cacheInfo(EmulatorInfo &result) const
{
    {   SYNCHRONIZED

        result.state = state;
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

i64
Emulator::get(Opt opt, isize objid) const
{
    return main.get(opt, objid);
}

void
Emulator::check(Opt opt, i64 value, const std::vector<isize> objids)
{
    return main.check(opt, value, objids);
}

void
Emulator::set(Opt opt, i64 value, const std::vector<isize> objids)
{
    return main.set(opt, value, objids);
}

void
Emulator::set(Opt opt, const string &value, const std::vector<isize> objids)
{
    return main.set(opt, value, objids);
}

void
Emulator::set(const string &opt, const string &value, const std::vector<isize> objids)
{
    return main.set(opt, value, objids);
}

void
Emulator::set(ConfigScheme scheme)
{
    main.set(scheme);
}

void
Emulator::revertToDefaultConfig()
{
    // Power off
    powerOff();

    // Setup the default configuration
    main.resetConfig();
    ahead.resetConfig();

    // Perform a hard reset
    main.hardReset();
    ahead.hardReset();
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
Emulator::shouldWarp() const
{
    auto &config = main.getConfig();

    if (main.cpu.clock < C64::sec(config.warpBoot)) {

        return true;

    } else {

        switch (config.warpMode) {

            case Warp::AUTO:     return main.iec.isTransferring();
            case Warp::NEVER:    return false;
            case Warp::ALWAYS:   return true;

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
    auto elapsed = utl::Time::now() - baseTime;

    // Compute which frame should be reached by now
    auto target = elapsed.asNanoseconds() * i64(main.refreshRate()) / 1000000000;

    // Compute the number of missing frames
    return isize(target - frameCounter);
}

void
Emulator::computeFrame()
{
    auto &config = main.getConfig();

    if (config.runAhead > 0) {

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
Emulator::isReady()
{
    main.isReady();
}

void
Emulator::cloneRunAheadInstance()
{
    stats.clones++;

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
    assert(main.config.runAhead > 0);

    auto &config = main.getConfig();

    // Clone the main instance
    if (RUA_DEBUG) {
        utl::StopWatch watch("Run-ahead: Clone");
        cloneRunAheadInstance();
    } else {
        cloneRunAheadInstance();
    }

    // Advance to the proper frame
    if (RUA_DEBUG) {
        utl::StopWatch watch("Run-ahead: Fast-forward");
        ahead.fastForward(config.runAhead - 1);
    } else {
        ahead.fastForward(config.runAhead - 1);
    }
}

void
Emulator::hardReset()
{
    main.hardReset();
}

void
Emulator::softReset()
{
    main.softReset();
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
Emulator::stepCycle()
{
    if (isRunning()) return;
    main.setFlag(RL::SINGLE_CYCLE);
    run();
}

void
Emulator::finishLine()
{
    if (isRunning()) return;
    main.setFlag(RL::FINISH_LINE);
    run();
}

void
Emulator::finishFrame()
{
    if (isRunning()) return;
    main.setFlag(RL::FINISH_FRAME);
    run();
}

/*
u32 *
Emulator::oldGetTexture() const
{
    if (isRunning()) {

        // In run-ahead mode, return the texture from the run-ahead instance
        if (main.config.runAhead > 0) {
            return ahead.videoPort.oldGetTexture();
        }

        // In run-behind mode, return a texture from the texture buffer
        if (main.config.runAhead < 0) {
            return main.videoPort.oldGetTexture(main.config.runAhead);
        }
    }

    // Return the most recent texture from the main instance
    return main.videoPort.oldGetTexture();
}
*/

const Texture &
Emulator::getTexture() const
{
    if (isRunning()) {

        // In run-ahead mode, return the texture from the run-ahead instance
        if (main.config.runAhead > 0) {
            return ahead.videoPort.getTexture();
        }

        // In run-behind mode, return a texture from the texture buffer
        if (main.config.runAhead < 0) {
            return main.videoPort.getTexture(main.config.runAhead);
        }
    }

    // Return the most recent texture from the main instance
    return main.videoPort.getTexture();
}

/*
u32 *
Emulator::oldGetDmaTexture() const
{
    if (isRunning()) {

        // In run-ahead mode, return the texture from the run-ahead instance
        if (main.config.runAhead > 0) {
            return ahead.videoPort.oldGetDmaTexture();
        }
    }

    // Return the most recent texture from the main instance
    return main.videoPort.oldGetDmaTexture();
}
*/

const Texture &
Emulator::getDmaTexture() const
{
    if (isRunning()) {

        // In run-ahead mode, return the texture from the run-ahead instance
        if (main.config.runAhead > 0) {
            return ahead.videoPort.getDmaTexture();
        }

        // In run-behind mode, return a texture from the texture buffer
        if (main.config.runAhead < 0) {
            return main.videoPort.getDmaTexture(main.config.runAhead);
        }
    }

    // Return the most recent texture from the main instance
    return main.videoPort.getDmaTexture();
}

void
Emulator::put(const Command &cmd)
{
    cmdQueue.put(cmd);
}

int
Emulator::getDebugVariable(DebugFlag flag)
{
#ifdef NDEBUG

    throw CoreError(CoreError::OPT_UNSUPPORTED, "Debug variables are only accessible in debug builds.");

#else

    switch (flag) {

        case DebugFlag::XFILES:           return XFILES;
        case DebugFlag::CNF_DEBUG:        return CNF_DEBUG;
        case DebugFlag::DEF_DEBUG:        return DEF_DEBUG;

        case DebugFlag::RUN_DEBUG:        return RUN_DEBUG;
        case DebugFlag::TIM_DEBUG:        return TIM_DEBUG;
        case DebugFlag::WARP_DEBUG:       return WARP_DEBUG;
        case DebugFlag::CMD_DEBUG:        return CMD_DEBUG;
        case DebugFlag::MSG_DEBUG:        return MSG_DEBUG;
        case DebugFlag::SNP_DEBUG:        return SNP_DEBUG;

        case DebugFlag::RUA_DEBUG:        return RUA_DEBUG;
        case DebugFlag::RUA_CHECKSUM:     return RUA_CHECKSUM;
        case DebugFlag::RUA_ON_STEROIDS:  return RUA_ON_STEROIDS;

        case DebugFlag::CPU_DEBUG:        return CPU_DEBUG;
        case DebugFlag::IRQ_DEBUG:        return IRQ_DEBUG;

        case DebugFlag::MEM_DEBUG:        return MEM_DEBUG;

        case DebugFlag::CIA_DEBUG:        return CIA_DEBUG;
        case DebugFlag::CIAREG_DEBUG:     return CIAREG_DEBUG;

        case DebugFlag::VICII_DEBUG:      return VICII_DEBUG;
        case DebugFlag::VICII_REG_DEBUG:  return VICII_REG_DEBUG;
        case DebugFlag::VICII_SAFE_MODE:  return VICII_SAFE_MODE;
        case DebugFlag::VICII_STATS:      return VICII_STATS;
        case DebugFlag::RASTERIRQ_DEBUG:  return RASTERIRQ_DEBUG;

        case DebugFlag::SID_DEBUG:        return SID_DEBUG;
        case DebugFlag::SID_EXEC:         return SID_EXEC;
        case DebugFlag::SIDREG_DEBUG:     return SIDREG_DEBUG;
        case DebugFlag::AUD_DEBUG:        return AUD_DEBUG;
        case DebugFlag::AUDBUF_DEBUG:     return AUDBUF_DEBUG;
        case DebugFlag::AUDVOL_DEBUG:     return AUDVOL_DEBUG;

        case DebugFlag::VIA_DEBUG:        return VIA_DEBUG;
        case DebugFlag::PIA_DEBUG:        return PIA_DEBUG;
        case DebugFlag::SER_DEBUG:        return SER_DEBUG;
        case DebugFlag::DSK_DEBUG:        return DSK_DEBUG;
        case DebugFlag::DSKCHG_DEBUG:     return DSKCHG_DEBUG;
        case DebugFlag::GCR_DEBUG:        return GCR_DEBUG;
        case DebugFlag::FS_DEBUG:         return FS_DEBUG;
        case DebugFlag::PAR_DEBUG:        return PAR_DEBUG;

        case DebugFlag::CRT_DEBUG:        return CRT_DEBUG;
        case DebugFlag::FILE_DEBUG:       return FILE_DEBUG;

        case DebugFlag::JOY_DEBUG:        return JOY_DEBUG;
        case DebugFlag::DRV_DEBUG:        return DRV_DEBUG;
        case DebugFlag::TAP_DEBUG:        return TAP_DEBUG;
        case DebugFlag::KBD_DEBUG:        return KBD_DEBUG;
        case DebugFlag::PRT_DEBUG:        return PRT_DEBUG;
        case DebugFlag::EXP_DEBUG:        return EXP_DEBUG;
        case DebugFlag::USR_DEBUG:        return USR_DEBUG;

        case DebugFlag::RSH_DEBUG:        return RSH_DEBUG;
        case DebugFlag::REU_DEBUG:        return REU_DEBUG;
        case DebugFlag::SCK_DEBUG:        return SCK_DEBUG;
        case DebugFlag::SRV_DEBUG:        return SRV_DEBUG;
        case DebugFlag::GDB_DEBUG:        return GDB_DEBUG;
        case DebugFlag::DAP_DEBUG:        return DAP_DEBUG;

        case DebugFlag::FORCE_LAUNCH_ERROR:       return FORCE_LAUNCH_ERROR;
        case DebugFlag::FORCE_ROM_MISSING:        return FORCE_ROM_MISSING;
        case DebugFlag::FORCE_MEGA64_MISMATCH:    return FORCE_MEGA64_MISMATCH;
        case DebugFlag::FORCE_SNAP_TOO_OLD:       return FORCE_SNAP_TOO_OLD;
        case DebugFlag::FORCE_SNAP_TOO_NEW:       return FORCE_SNAP_TOO_NEW;
        case DebugFlag::FORCE_SNAP_IS_BETA:       return FORCE_SNAP_IS_BETA;
        case DebugFlag::FORCE_SNAP_CORRUPTED:     return FORCE_SNAP_CORRUPTED;
        case DebugFlag::FORCE_CRT_UNKNOWN:        return FORCE_CRT_UNKNOWN;
        case DebugFlag::FORCE_CRT_UNSUPPORTED:    return FORCE_CRT_UNSUPPORTED;

        default:
            throw CoreError(CoreError::OPT_UNSUPPORTED,
                        "Unhandled debug variable: " + string(DebugFlagEnum::key(flag)));
    }

#endif
}

void
Emulator::setDebugVariable(DebugFlag flag, bool val)
{
#ifdef NDEBUG

    throw CoreError(CoreError::OPT_UNSUPPORTED, "Debug variables are only accessible in debug builds.");

#else

    switch (flag) {

        case DebugFlag::XFILES:           XFILES          = val; break;
        case DebugFlag::CNF_DEBUG:        CNF_DEBUG       = val; break;
        case DebugFlag::DEF_DEBUG:        DEF_DEBUG       = val; break;

        case DebugFlag::RUN_DEBUG:        RUN_DEBUG       = val; break;
        case DebugFlag::TIM_DEBUG:        TIM_DEBUG       = val; break;
        case DebugFlag::WARP_DEBUG:       WARP_DEBUG      = val; break;
        case DebugFlag::CMD_DEBUG:        CMD_DEBUG       = val; break;
        case DebugFlag::MSG_DEBUG:        MSG_DEBUG       = val; break;
        case DebugFlag::SNP_DEBUG:        SNP_DEBUG       = val; break;

        case DebugFlag::RUA_DEBUG:        RUA_DEBUG       = val; break;
        case DebugFlag::RUA_CHECKSUM:     RUA_CHECKSUM    = val; break;
        case DebugFlag::RUA_ON_STEROIDS:  RUA_ON_STEROIDS = val; break;

        case DebugFlag::CPU_DEBUG:        CPU_DEBUG       = val; break;
        case DebugFlag::IRQ_DEBUG:        IRQ_DEBUG       = val; break;

        case DebugFlag::MEM_DEBUG:        MEM_DEBUG       = val; break;

        case DebugFlag::CIA_DEBUG:        CIA_DEBUG       = val; break;
        case DebugFlag::CIAREG_DEBUG:     CIAREG_DEBUG    = val; break;

        case DebugFlag::VICII_DEBUG:      VICII_DEBUG     = val; break;
        case DebugFlag::VICII_REG_DEBUG:  VICII_REG_DEBUG = val; break;
        case DebugFlag::VICII_SAFE_MODE:  VICII_SAFE_MODE = val; break;
        case DebugFlag::VICII_STATS:      VICII_STATS     = val; break;
        case DebugFlag::RASTERIRQ_DEBUG:  RASTERIRQ_DEBUG = val; break;

        case DebugFlag::SID_DEBUG:        SID_DEBUG       = val; break;
        case DebugFlag::SID_EXEC:         SID_EXEC        = val; break;
        case DebugFlag::SIDREG_DEBUG:     SIDREG_DEBUG    = val; break;
        case DebugFlag::AUD_DEBUG:        AUD_DEBUG       = val; break;
        case DebugFlag::AUDBUF_DEBUG:     AUDBUF_DEBUG    = val; break;
        case DebugFlag::AUDVOL_DEBUG:     AUDVOL_DEBUG    = val; break;

        case DebugFlag::VIA_DEBUG:        VIA_DEBUG       = val; break;
        case DebugFlag::PIA_DEBUG:        PIA_DEBUG       = val; break;
        case DebugFlag::SER_DEBUG:        SER_DEBUG       = val; break;
        case DebugFlag::DSK_DEBUG:        DSK_DEBUG       = val; break;
        case DebugFlag::DSKCHG_DEBUG:     DSKCHG_DEBUG    = val; break;
        case DebugFlag::GCR_DEBUG:        GCR_DEBUG       = val; break;
        case DebugFlag::FS_DEBUG:         FS_DEBUG        = val; break;
        case DebugFlag::PAR_DEBUG:        PAR_DEBUG       = val; break;

        case DebugFlag::CRT_DEBUG:        CRT_DEBUG       = val; break;
        case DebugFlag::FILE_DEBUG:       FILE_DEBUG      = val; break;

        case DebugFlag::JOY_DEBUG:        JOY_DEBUG       = val; break;
        case DebugFlag::DRV_DEBUG:        DRV_DEBUG       = val; break;
        case DebugFlag::TAP_DEBUG:        TAP_DEBUG       = val; break;
        case DebugFlag::KBD_DEBUG:        KBD_DEBUG       = val; break;
        case DebugFlag::PRT_DEBUG:        PRT_DEBUG       = val; break;
        case DebugFlag::EXP_DEBUG:        EXP_DEBUG       = val; break;
        case DebugFlag::USR_DEBUG:        USR_DEBUG       = val; break;

        case DebugFlag::RSH_DEBUG:        RSH_DEBUG       = val; break;
        case DebugFlag::REU_DEBUG:        REU_DEBUG       = val; break;
        case DebugFlag::SCK_DEBUG:        SCK_DEBUG       = val; break;
        case DebugFlag::SRV_DEBUG:        SRV_DEBUG       = val; break;
        case DebugFlag::GDB_DEBUG:        GDB_DEBUG       = val; break;
        case DebugFlag::DAP_DEBUG:        DAP_DEBUG       = val; break;

        case DebugFlag::FORCE_LAUNCH_ERROR:       FORCE_LAUNCH_ERROR = val; break;
        case DebugFlag::FORCE_ROM_MISSING:        FORCE_ROM_MISSING = val; break;
        case DebugFlag::FORCE_MEGA64_MISMATCH:    FORCE_MEGA64_MISMATCH = val; break;
        case DebugFlag::FORCE_SNAP_TOO_OLD:       FORCE_SNAP_TOO_OLD = val; break;
        case DebugFlag::FORCE_SNAP_TOO_NEW:       FORCE_SNAP_TOO_NEW = val; break;
        case DebugFlag::FORCE_SNAP_IS_BETA:       FORCE_SNAP_IS_BETA = val; break;
        case DebugFlag::FORCE_SNAP_CORRUPTED:     FORCE_SNAP_CORRUPTED = val; break;
        case DebugFlag::FORCE_CRT_UNKNOWN:        FORCE_CRT_UNKNOWN = val; break;
        case DebugFlag::FORCE_CRT_UNSUPPORTED:    FORCE_CRT_UNSUPPORTED = val; break;

        default:
            throw CoreError(CoreError::OPT_UNSUPPORTED,
                        "Unhandled debug variable: " + string(DebugFlagEnum::key(flag)));
    }
#endif
}

}
