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

Thread::Thread()
{

}

Thread::~Thread()
{
    printf("~Thread\n");
    join();
}

void
Thread::launch()
{
    // Make sure to call this function only once
    assert(!thread.joinable());

    // Start the thread and enter the main function
    thread = std::thread(&Thread::main, this);
}

void
Thread::resync()
{
    baseTime = util::Time::now();
    frameCounter = 0;
}

void
Thread::executeFrame()
{
    // Determine the number of overdue frames
    isize missing = warp ? 1 : missingFrames();

    if (std::abs(missing) <= 5) {

        // Execute all missing frames
        loadClock.go();
        for (isize i = 0; i < missing; i++, frameCounter++) execute();
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

    assert(missing == 0);

    // Set a timeout to prevent the thread from stalling
    auto timeout = util::Time(i64(2000000000.0 / refreshRate()));

    // Wait for the next pulse
    waitForWakeUp(timeout);
}

void
Thread::main()
{
    debug(RUN_DEBUG, "main()\n");

    baseTime = util::Time::now();

    while (1) {

        updateWarp();

        if (isRunning()) {

            executeFrame();
        }

        if (!warp || !isRunning()) {

            sleep();
        }
        
        // Are we requested to change state?
        if (stateChangeRequest.test()) {

            switchState(newState);
            stateChangeRequest.clear();
            stateChangeRequest.notify_one();

            if (state == EXEC_HALTED) return;
        }

        // Compute the CPU load once in a while
        if (frameCounter % 32 == 0) {

            auto used  = loadClock.getElapsedTime().asSeconds();
            auto total = nonstopClock.getElapsedTime().asSeconds();

            cpuLoad = used / total;

            loadClock.restart();
            loadClock.stop();
            nonstopClock.restart();
        }
    }
}

void
Thread::switchState(ExecutionState newState)
{
    assert(isEmulatorThread());

    if (state == EXEC_OFF && newState == EXEC_PAUSED) {

        stateChange(EXEC_POWER_ON);
        state = EXEC_PAUSED;

    } else if (state == EXEC_OFF && newState == EXEC_RUNNING) {

        stateChange(EXEC_POWER_ON);
        state = EXEC_PAUSED;

        stateChange(EXEC_RUN);
        state = EXEC_RUNNING;

    } else if (state == EXEC_PAUSED && newState == EXEC_OFF) {

        stateChange(EXEC_POWER_OFF);
        state = EXEC_OFF;

    } else if (state == EXEC_PAUSED && newState == EXEC_RUNNING) {

        stateChange(EXEC_RUN);
        state = EXEC_RUNNING;

    } else if (state == EXEC_RUNNING && newState == EXEC_OFF) {

        stateChange(EXEC_PAUSE);
        state = EXEC_PAUSED;

        stateChange(EXEC_POWER_OFF);
        state = EXEC_OFF;

    } else if (state == EXEC_RUNNING && newState == EXEC_PAUSED) {

        stateChange(EXEC_PAUSE);
        state = EXEC_PAUSED;

    } else if (state == EXEC_RUNNING && newState == EXEC_SUSPENDED) {

        stateChange(EXEC_SUSPEND);
        state = EXEC_SUSPENDED;

    } else if (state == EXEC_SUSPENDED && newState == EXEC_RUNNING) {

        stateChange(EXEC_RESUME);
        state = EXEC_RUNNING;

    } else if (newState == EXEC_HALTED) {

        stateChange(EXEC_HALT);
        state = EXEC_HALTED;

    } else {

        // Invalid state transition
        fatalError;
    }

    debug(RUN_DEBUG, "Changed state to %s\n", ExecutionStateEnum::key(state));
}

void
Thread::switchTrack(bool state, u8 source)
{
    assert(source >= 0 && source < 8);
    assert(isEmulatorThread() || !isRunning());

    u8 old = track;
    state ? SET_BIT(track, source) : CLR_BIT(track, source);

    if (bool(old) != bool(track)) {
        trackOnOffDelegate(track);
    }
}

void
Thread::powerOn()
{
    debug(RUN_DEBUG, "powerOn()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());
    
    if (isPoweredOff()) {
        
        // Request a state change and wait until the new state has been reached
        changeStateTo(EXEC_PAUSED);
    }
}

void
Thread::powerOff()
{
    debug(RUN_DEBUG, "powerOff()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());
    
    if (!isPoweredOff()) {

        // Request a state change and wait until the new state has been reached
        changeStateTo(EXEC_OFF);
    }
}

void
Thread::run()
{
    debug(RUN_DEBUG, "run()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());

    if (!isRunning()) {

        // Throw an exception if the emulator is not ready to run
        readyToGo();

        // Request a state change and wait until the new state has been reached
        changeStateTo(EXEC_RUNNING);
    }
}

void
Thread::pause()
{
    debug(RUN_DEBUG, "pause()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());
    
    if (isRunning()) {

        // Request a state change and wait until the new state has been reached
        changeStateTo(EXEC_PAUSED);
    }
}

void
Thread::halt()
{
    assert(!isEmulatorThread());

    changeStateTo(EXEC_HALTED);
    join();
}

void
Thread::trackOn(isize source)
{
    SUSPENDED switchTrack(true, u8(source));
}

void
Thread::trackOff(isize source)
{
    SUSPENDED switchTrack(false, u8(source));
}

void
Thread::changeStateTo(ExecutionState requestedState)
{
    assert(!isEmulatorThread());
    assert(stateChangeRequest.test() == false);

    // Assign new state
    newState = requestedState;

    // Request the change
    stateChangeRequest.test_and_set();
    assert(stateChangeRequest.test() == true);

    // Wait until the change has been performed
    stateChangeRequest.wait(true);
    assert(stateChangeRequest.test() == false);
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
    debug(RUN_DEBUG, "Suspending (%ld)...\n", suspendCounter);
    
    if (suspendCounter || isRunning()) {

        suspendCounter++;
        assert(state == EXEC_RUNNING || state == EXEC_SUSPENDED);
        changeStateTo(EXEC_SUSPENDED);
    }
}

void
Thread::resume()
{
    debug(RUN_DEBUG, "Resuming (%ld)...\n", suspendCounter);

    if (suspendCounter && --suspendCounter == 0) {
        
        assert(state == EXEC_SUSPENDED);
        changeStateTo(EXEC_RUNNING);
        run();
    }
}

}
