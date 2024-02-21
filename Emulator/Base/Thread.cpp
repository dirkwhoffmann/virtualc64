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

}

void
Thread::launch()
{
    // Make sure to call this function only once
    assert(!thread.joinable());

    // Start the thread and enter the main function
    thread = std::thread(&Thread::runLoop, this);
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

    while (state != STATE_HALTED) {

        // Update state
        update();

        // Compute missing frames
        execute();

        // Synchronize timing
        sleep();

        // Compute statistics
        computeStats();
    }
}

void
Thread::switchState(EmulatorState newState)
{
    // Only proceed if the state changes
    if (state == newState) return;

    if (state == STATE_OFF && newState == STATE_PAUSED) {

        _powerOn();
        state = STATE_PAUSED;

    } else if (state == STATE_OFF && newState == STATE_RUNNING) {

        _powerOn();
        state = STATE_PAUSED;

        _run();
        state = STATE_RUNNING;

    } else if (state == STATE_PAUSED && newState == STATE_OFF) {

        _powerOff();
        state = STATE_OFF;

    } else if (state == STATE_PAUSED && newState == STATE_RUNNING) {

        _run();
        state = STATE_RUNNING;

    } else if (state == STATE_RUNNING && newState == STATE_OFF) {

        _pause();
        state = STATE_PAUSED;

        _powerOff();
        state = STATE_OFF;

    } else if (state == STATE_RUNNING && newState == STATE_PAUSED) {

        _pause();
        state = STATE_PAUSED;

    } else if (state == STATE_RUNNING && newState == STATE_SUSPENDED) {

        state = STATE_SUSPENDED;

    } else if (state == STATE_SUSPENDED && newState == STATE_RUNNING) {

        state = STATE_RUNNING;

    } else if (newState == STATE_HALTED) {

        _halt();
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
    debug(RUN_DEBUG, "powerOn()\n");

    if (isPoweredOff()) {

        switchState(STATE_PAUSED);
    }
}

void
Thread::powerOff()
{
    debug(RUN_DEBUG, "powerOff()\n");

    if (!isPoweredOff()) {

        switchState(STATE_OFF);
    }
}

void
Thread::run()
{
    debug(RUN_DEBUG, "run()\n");

    if (!isRunning()) {

        // Throw an exception if the emulator is not ready to run
        isReady();

        switchState(STATE_RUNNING);
    }
}

void
Thread::pause()
{
    debug(RUN_DEBUG, "pause()\n");

    if (isRunning()) {

        switchState(STATE_PAUSED);
    }
}

void
Thread::halt()
{
    switchState(STATE_HALTED);
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
        switchState(STATE_SUSPENDED);
    }
}

void
Thread::resume()
{
    debug(RUN_DEBUG, "Resuming (%ld)...\n", suspendCounter);

    if (suspendCounter && --suspendCounter == 0) {
        
        switchState(STATE_RUNNING);
        run();
    }
}

}
