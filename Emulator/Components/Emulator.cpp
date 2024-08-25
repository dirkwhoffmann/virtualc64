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

void
Emulator::process(const Cmd &cmd)
{

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
            throw Error(VC64ERROR_OPT_INV_ID, "0..." + std::to_string(targets.size() - 1));
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
    set(opt, OptionParser::parse(opt, value));
}

void
Emulator::set(Option opt, const string &value, isize id)
{
    set(opt, OptionParser::parse(opt, value), id);
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

        powerOff();
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
    std::vector<Configurable *> result;

    main.routeOption(opt, result);

    assert(!result.empty());
    return result;
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

}
