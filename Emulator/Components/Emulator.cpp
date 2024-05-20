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
    // Initialize all components
    resetConfig();
    host.resetConfig();
    main.initialize();
    ahead.initialize();

    // Register the audio sample provider
    C64::audioPort.connectDataSource(&main.sidBridge);

    // Perform a hard reset
    main.hardReset();
    ahead.hardReset();

    assert(isInitialized());
}

bool 
Emulator::isInitialized() const
{
    return main.vic.vicfunc[1] != nullptr;
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
    // Power off the emulator
    powerOff();

    // Put all components into their initial state
    initialize();
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
    Cmd cmd;

    auto drive = [&]() -> Drive& { return cmd.value == DRIVE9 ? main.drive9 : main.drive8; };
    // auto port = [&]() -> ControlPort& { return cmd.value == PORT_2 ? main.port2 : main.port1; };

    shouldWarp() ? warpOn() : warpOff();

    while (cmdQueue.poll(cmd)) {

        debug(CMD_DEBUG, "Command: %s\n", CmdTypeEnum::key(cmd.type));

        main.markAsDirty();

        switch (cmd.type) {
                
            case CMD_CPU_BRK:
            case CMD_CPU_NMI:
            case CMD_ALARM_ABS:
            case CMD_ALARM_REL:
                
                main.process(cmd);
                break;

            case CMD_KEY_PRESS:
            case CMD_KEY_RELEASE:
            case CMD_KEY_RELEASE_ALL:
            case CMD_KEY_TOGGLE:

                main.keyboard.processCommand(cmd);
                break;

            case CMD_DSK_TOGGLE_WP:
            case CMD_DSK_MODIFIED:
            case CMD_DSK_UNMODIFIED:

                drive().processCommand(cmd);
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

            case CMD_FOCUS:

                cmd.value ? main.focus() : main.unfocus();
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

        try {

            // Run the main instance
            main.execute();

            // Recreate the run-ahead instance if necessary
            if (main.isDirty || RUA_ON_STEROIDS) recreateRunAheadInstance();

            // Run the runahead instance
            ahead.execute();

        } catch (StateChangeException &) {

            ahead.markAsDirty();
            throw;
        }

    } else {

        // Only run the main instance
        main.execute();
    }
}

void 
Emulator::recreateRunAheadInstance()
{
    debug(RUA_DEBUG, "%lld: Recomputing the run-ahead instance\n", main.frame);

    clones++;

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
    return config.runAhead && isRunning() ?
    ahead.vic.getTexture() :
    main.vic.getTexture();
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
