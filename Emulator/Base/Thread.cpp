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
Thread::updateWarp()
{
    shouldWarp() ? SET_BIT(warp, 7) : CLR_BIT(warp, 7);

    if (bool(warp) != bool(oldWarp)) {

        stateChange(warp ? TRANSITION_WARP_ON : TRANSITION_WARP_OFF);
        oldWarp = warp;
    }
}

void
Thread::updateTrack()
{
    shouldTrack() ? SET_BIT(track, 7) : CLR_BIT(track, 7);

    if (bool(track) != bool(oldTrack)) {

        stateChange(track ? TRANSITION_TRACK_ON : TRANSITION_TRACK_OFF);
        oldTrack = track;
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

        // Execute all missing frames
        loadClock.go();
        for (isize i = 0; i < missing; i++, frameCounter++) computeFrame();
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
    auto timeout = util::Time(i64(2000000000.0 / refreshRate()));

    // Wait for the next pulse
    waitForWakeUp(timeout);
}

double 
Thread::computeCpuLoad()
{
    auto used  = loadClock.getElapsedTime().asSeconds();
    auto total = nonstopClock.getElapsedTime().asSeconds();

    loadClock.restart();
    loadClock.stop();
    nonstopClock.restart();

    cpuLoad = 0.3 * cpuLoad + 0.7 * used / total;
    return cpuLoad;
}

void
Thread::main()
{
    debug(RUN_DEBUG, "main()\n");

    baseTime = util::Time::now();

    while (1) {

        // Update warp and track state
        updateWarp();
        updateTrack();

        // Compute missing frames
        execute();

        // Synchronize timing
        sleep();

        // Are we requested to change state?
        if (stateChangeRequest.test()) {

            switchState(newState);
            stateChangeRequest.clear();
            stateChangeRequest.notify_one();

            if (state == STATE_HALTED) return;
        }
    }
}

void
Thread::switchState(EmulatorState newState)
{
    assert(isEmulatorThread());

    if (state == STATE_OFF && newState == STATE_PAUSED) {

        stateChange(TRANSITION_POWER_ON);
        state = STATE_PAUSED;

    } else if (state == STATE_OFF && newState == STATE_RUNNING) {

        stateChange(TRANSITION_POWER_ON);
        state = STATE_PAUSED;

        stateChange(TRANSITION_RUN);
        state = STATE_RUNNING;

    } else if (state == STATE_PAUSED && newState == STATE_OFF) {

        stateChange(TRANSITION_POWER_OFF);
        state = STATE_OFF;

    } else if (state == STATE_PAUSED && newState == STATE_RUNNING) {

        stateChange(TRANSITION_RUN);
        state = STATE_RUNNING;

    } else if (state == STATE_RUNNING && newState == STATE_OFF) {

        stateChange(TRANSITION_PAUSE);
        state = STATE_PAUSED;

        stateChange(TRANSITION_POWER_OFF);
        state = STATE_OFF;

    } else if (state == STATE_RUNNING && newState == STATE_PAUSED) {

        stateChange(TRANSITION_PAUSE);
        state = STATE_PAUSED;

    } else if (state == STATE_RUNNING && newState == STATE_SUSPENDED) {

        stateChange(TRANSITION_SUSPEND);
        state = STATE_SUSPENDED;

    } else if (state == STATE_SUSPENDED && newState == STATE_RUNNING) {

        stateChange(TRANSITION_RESUME);
        state = STATE_RUNNING;

    } else if (newState == STATE_HALTED) {

        stateChange(TRANSITION_HALT);
        state = STATE_HALTED;

    } else {

        // Invalid state transition
        fatalError;
    }

    debug(RUN_DEBUG, "Changed state to %s\n", EmulatorStateEnum::key(state));
}

void
Thread::powerOn()
{
    assert(!isEmulatorThread());
    debug(RUN_DEBUG, "powerOn()\n");

    if (isPoweredOff()) {
        
        // Request a state change and wait until the new state has been reached
        changeStateTo(STATE_PAUSED);
    }
}

void
Thread::powerOff()
{
    assert(!isEmulatorThread());
    debug(RUN_DEBUG, "powerOff()\n");

    if (!isPoweredOff()) {

        // Request a state change and wait until the new state has been reached
        changeStateTo(STATE_OFF);
    }
}

void
Thread::run()
{
    assert(!isEmulatorThread());
    debug(RUN_DEBUG, "run()\n");

    if (!isRunning()) {

        // Throw an exception if the emulator is not ready to run
        isReady();

        // Request a state change and wait until the new state has been reached
        changeStateTo(STATE_RUNNING);
    }
}

void
Thread::pause()
{
    assert(!isEmulatorThread());
    debug(RUN_DEBUG, "pause()\n");

    if (isRunning()) {

        // Request a state change and wait until the new state has been reached
        changeStateTo(STATE_PAUSED);
    }
}

void
Thread::halt()
{
    assert(!isEmulatorThread());

    changeStateTo(STATE_HALTED);
    join();
}

void
Thread::warpOn(isize source)
{
    assert(!isEmulatorThread());
    assert(source < 7);

    SUSPENDED SET_BIT(warp, source);
}

void
Thread::warpOff(isize source)
{
    assert(!isEmulatorThread());
    assert(source < 7);

    SUSPENDED CLR_BIT(warp, source);
}

void
Thread::trackOn(isize source)
{
    assert(!isEmulatorThread());
    assert(source < 7);

    SUSPENDED SET_BIT(track, source);
}

void
Thread::trackOff(isize source)
{
    assert(!isEmulatorThread());
    assert(source < 7);

    SUSPENDED CLR_BIT(track, source);
}

void
Thread::changeStateTo(EmulatorState requestedState)
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
        changeStateTo(STATE_SUSPENDED);
    }
}

void
Thread::resume()
{
    debug(RUN_DEBUG, "Resuming (%ld)...\n", suspendCounter);

    if (suspendCounter && --suspendCounter == 0) {
        
        changeStateTo(STATE_RUNNING);
        run();
    }
}

}
