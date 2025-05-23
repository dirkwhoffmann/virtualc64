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
#include "Thread.h"
#include "Chrono.h"
#include <iostream>

namespace vc64 {

void
Thread::launch()
{
    assert(!isLaunched());

    // Start the thread and enter the main function
    thread = std::thread(&Thread::runLoop, this);

    assert(isLaunched());
}

void 
Thread::assertLaunched()
{
    if (!isLaunched()) {

        throw std::runtime_error(string("The emulator thread hasn't been lauchend yet. "
                                        "Missing call to launch()."));
    }
}

void
Thread::resync()
{
    baseTime = util::Time::now();
    frameCounter = 0;
}

void
Thread::execute()
{
    // Only proceed if the emulator is running
    if (!isRunning()) return;

    // Determine the number of overdue frames
    isize missing = warp ? 1 : missingFrames();

    if (std::abs(missing) <= 5) {

        loadClock.go();
        try {

            // Execute all missing frames
            for (isize i = 0; i < missing; i++, frameCounter++) computeFrame();

        } catch (StateChangeException &exc) {

            // Interruption
            switchState((ExecState)exc.data);
        }
        loadClock.stop();

    } else {

        // The emulator got out of sync
        if (missing > 0) {
            warn("Emulation is way too slow (%ld frames behind)\n", missing);
        } else {
            warn("Emulation is way too fast (%ld time slices ahead)\n", -missing);
        }

        resync();
    }
}

void
Thread::sleep()
{
    // Don't sleep if the emulator is running in warp mode
    if (warp && isRunning()) return;

    // Set a timeout to prevent the thread from stalling
    auto timeout = util::Time::milliseconds(50);

    // Wait for the next pulse
    waitForWakeUp(timeout);
}

void
Thread::computeStats()
{
    if (statsCounter++ == 32) {

        auto used  = loadClock.getElapsedTime().asSeconds();
        auto total = nonstopClock.getElapsedTime().asSeconds();

        loadClock.restart();
        loadClock.stop();
        nonstopClock.restart();

        cpuLoad = 0.3 * cpuLoad + 0.7 * used / total;
        fps = 0.3 * fps + 0.7 * statsCounter / total;

        statsCounter = 0;
    }
}

void
Thread::runLoop()
{
    baseTime = util::Time::now();

    while (state != ExecState::HALTED) {

        // Prepare for the next frame
        update();

        // Compute missing frames
        execute();

        // Change state if requested
        if (stateChangeRequest.test()) {

            switchState(newState);
            stateChangeRequest.clear();
            stateChangeRequest.notify_one();
        }

        // Synchronize timing
        sleep();

        // Compute statistics
        computeStats();
    }
}

void
Thread::switchState(ExecState newState)
{
    assert(isEmulatorThread());

    auto invalid = [&]() {
        
        assert(false);
        fatal("Invalid state transition: %s -> %s\n",
              ExecStateEnum::key(state), ExecStateEnum::key(newState));
    };

    debug(RUN_DEBUG,
          "switchState: %s -> %s\n",
          ExecStateEnum::key(state), ExecStateEnum::key(newState));

    while (state != newState) {

        switch (newState) {

            case ExecState::OFF:

                switch (state) {

                    case ExecState::PAUSED:      state = ExecState::OFF; _powerOff(); break;
                    case ExecState::RUNNING:
                    case ExecState::SUSPEND:     state = ExecState::PAUSED; _pause(); break;

                    default:
                        invalid();
                }
                break;

            case ExecState::PAUSED:

                switch (state) {

                    case ExecState::OFF:         state = ExecState::PAUSED; _powerOn(); break;
                    case ExecState::RUNNING:
                    case ExecState::SUSPEND:   state = ExecState::PAUSED; _pause(); break;

                    default:
                        invalid();
                }
                break;

            case ExecState::RUNNING:

                switch (state) {

                    case ExecState::OFF:         state = ExecState::PAUSED; _powerOn(); break;
                    case ExecState::PAUSED:      state = ExecState::RUNNING; _run(); break;
                    case ExecState::SUSPEND:   state = ExecState::PAUSED; break;

                    default:
                        invalid();
                }
                break;

            case ExecState::SUSPEND:

                switch (state) {

                    case ExecState::RUNNING:     state = ExecState::SUSPEND; break;
                    case ExecState::PAUSED:      break;

                    default:
                        invalid();
                }
                break;

            case ExecState::HALTED:

                switch (state) {

                    case ExecState::OFF:     state = ExecState::HALTED; _halt(); break;
                    case ExecState::PAUSED:  state = ExecState::OFF; _powerOff(); break;
                    case ExecState::RUNNING: state = ExecState::PAUSED; _pause(); break;

                    default:
                        invalid();
                }
                break;

            default:
                fatalError;
        }
    }

    debug(RUN_DEBUG, "switchState: %s\n", ExecStateEnum::key(state));
}

void
Thread::powerOn()
{
    debug(RUN_DEBUG, "powerOn()\n");

    if (isPoweredOff()) {

        changeStateTo(ExecState::PAUSED);
    }
}

void
Thread::powerOff()
{
    debug(RUN_DEBUG, "powerOff()\n");

    if (!isPoweredOff()) {

        changeStateTo(ExecState::OFF);
    }
}

void
Thread::run()
{
    debug(RUN_DEBUG, "run()\n");

    if (!isRunning()) {

        // Throw an exception if the emulator is not ready to run
        isReady();

        changeStateTo(ExecState::RUNNING);
    }
}

void
Thread::pause()
{
    debug(RUN_DEBUG, "pause()\n");

    if (isRunning()) {

        changeStateTo(ExecState::PAUSED);
    }
}

void
Thread::halt()
{
    if (state != ExecState::UNINIT && state != ExecState::HALTED) {
        
        debug(RUN_DEBUG, "Switching to HALT state...\n");
        changeStateTo(ExecState::HALTED);

        debug(RUN_DEBUG, "Waiting for the emulator thread to terminate...\n");
        join();

        debug(RUN_DEBUG, "Emulator is halted.\n");
        assert(state == ExecState::HALTED);
    }
}

void
Thread::warpOn(isize source)
{
    assert(source < 7);

    if (!GET_BIT(warp, source)) {

        SUSPENDED

        auto old = warp;
        SET_BIT(warp, source);
        if (!!old != !!warp) _warpOn();
    }
}

void
Thread::warpOff(isize source)
{
    assert(source < 7);

    if (GET_BIT(warp, source)) {

        SUSPENDED

        auto old = warp;
        CLR_BIT(warp, source);
        if (!!old != !!warp) _warpOff();
    }
}

void
Thread::trackOn(isize source)
{
    assert(source < 7);

    if (!GET_BIT(track, source)) {

        SUSPENDED

        auto old = track;
        SET_BIT(track, source);
        if (!!old != !!track) _trackOn();
    }
}

void
Thread::trackOff(isize source)
{
    assert(source < 7);

    if (GET_BIT(track, source)) {

        SUSPENDED

        auto old = track;
        CLR_BIT(track, source);
        if (!!old != !!track) _trackOff();
    }
}

void
Thread::changeStateTo(ExecState requestedState)
{
    assertLaunched();

    if (isEmulatorThread()) {

        // Switch immediately
        switchState(requestedState);
        assert(state == requestedState);

    } else {

        // Remember the requested state
        newState = requestedState;

        // Request the change
        assert(stateChangeRequest.test() == false);
        stateChangeRequest.test_and_set();
        assert(stateChangeRequest.test() == true);

        if (!isEmulatorThread()) {

            // Wait until the change has been performed
            stateChangeRequest.wait(true);
            assert(stateChangeRequest.test() == false);
        }
    }
}

void
Thread::wakeUp()
{
    trace(TIM_DEBUG, "wakeup: %lld us\n", wakeupClock.restart().asMicroseconds());
    Wakeable::wakeUp();
}

void
Thread::suspend()
{
    if (!isEmulatorThread()) {

        debug(RUN_DEBUG, "Suspending (%ld)...\n", suspendCounter);

        if (suspendCounter || isRunning()) {

            suspendCounter++;
            changeStateTo(ExecState::SUSPEND);
        }

    } else {

        debug(RUN_DEBUG, "Skipping suspend (%ld)...\n", suspendCounter);
    }
}

void
Thread::resume()
{
    if (!isEmulatorThread()) {

        debug(RUN_DEBUG, "Resuming (%ld)...\n", suspendCounter);

        if (suspendCounter && --suspendCounter == 0) {

            changeStateTo(ExecState::RUNNING);
            run();
        }

    } else {

        debug(RUN_DEBUG, "Skipping resume (%ld)...\n", suspendCounter);
    }
}

}
