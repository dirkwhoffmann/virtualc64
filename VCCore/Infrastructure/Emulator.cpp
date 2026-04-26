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
    if (force::LAUNCH_ERROR) throw CoreError(CoreError::LAUNCH);

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

        auto cs = getChannels();
        std::sort(cs.begin(), cs.end(),
                  [](const auto& a, const auto& b) { return a.name < b.name; });

        for (auto c : cs) {

            os << tab(c.name);
            if (c.level.has_value()) {
                os << LogLevelEnum::key(*c.level) << std::endl;
            } else {
                os << "-" << std::endl;
            }
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
            if (isDirty || debug::RUA_ON_STEROIDS) recreateRunAheadInstance();

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

    if (debug::RUA_CHECKSUM && ahead != main) {

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
    if (debug::RUA_DEBUG) {
        utl::StopWatch watch("Run-ahead: Clone");
        cloneRunAheadInstance();
    } else {
        cloneRunAheadInstance();
    }

    // Advance to the proper frame
    if (debug::RUA_DEBUG) {
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

}
