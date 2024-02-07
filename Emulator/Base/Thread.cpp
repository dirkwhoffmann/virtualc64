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
    // Wait until the thread has terminated
    join();
}

util::Time
Thread::frameDuration() const
{
    return util::Time(i64(1000000000.0 / refreshRate()));
}

util::Time
Thread::sliceDuration() const
{
    return util::Time(i64(1000000000.0 / refreshRate() / slicesPerFrame()));
}

isize
Thread::missingSlices() const
{
    if (getSyncMode() == SYNC_PULSED) {

        return slicesPerFrame();
    }
    if (getSyncMode() == SYNC_ADAPTIVE) {

        // Compute the elapsed time
        auto elapsed = util::Time::now() - baseTime;

        // Compute which slice should be reached by now
        auto target = slicesPerFrame() * elapsed.asNanoseconds() * i64(refreshRate()) / 1000000000;

        // Compute the number of missing slices
        return isize(target - sliceCounter);
    }

    return 0;
}

void
Thread::resync()
{
    targetTime = util::Time::now();
    baseTime = util::Time::now();
    deltaTime = 0;
    sliceCounter = 0;
    missing = 0;
}

template <SyncMode M> void
Thread::execute()
{
    if (missing > 0 || warp) {

        trace(TIM_DEBUG, "execute<%s>: %lld us\n", SyncModeEnum::key(M),
              execClock.restart().asMicroseconds());

        loadClock.go();

        execute();
        sliceCounter++;
        missing--;

        loadClock.stop();
    }
}

template <> void
Thread::sleep<SYNC_PERIODIC>()
{
    auto now = util::Time::now();

    // Don't sleep in warp mode
    if (warp) return;

    // Make sure the emulator is still in sync
    if ((now - targetTime).asMilliseconds() > 200) {
        warn("Emulation is way too slow: %f sec behind\n", (now - targetTime).asSeconds());
        resync();
    }
    if ((targetTime - now).asMilliseconds() > 200) {
        warn("Emulation is way too fast: %f sec ahead\n", (targetTime - now).asSeconds());
        resync();
    }

    // Sleep till the next sync point
    targetTime += sliceDuration();
    targetTime.sleepUntil();
    missing = 1;
}

template <> void
Thread::sleep<SYNC_PULSED>()
{
    // Only proceed if we're not running in warp mode
    if (warp) return;

    if (missing > 0) {

        // Wake up at the scheduled target time
        targetTime.sleepUntil();

        // Schedule the next execution
        targetTime += deltaTime;

    } else {

        // Set a timeout to prevent the thread from stalling
        auto timeout = util::Time(i64(2000000000.0 / refreshRate()));

        // Wait for the next pulse
        waitForWakeUp(timeout);

        // Determine the number of slices that are overdue
        missing = missingSlices();

        if (missing) {

            // Evenly distribute the missing slices till the next wakeup happens
            deltaTime = wakeupPeriod() / missing;
            targetTime = util::Time::now() + deltaTime;

            // Start over if the emulator got out of sync
            if (std::abs(missing) > 5 * slicesPerFrame()) {
                
                if (missing > 0) {
                    warn("Emulation is way too slow: %ld time slices behind\n", missing);
                } else {
                    warn("Emulation is way too fast: %ld time slices ahead\n", -missing);
                }

                resync();
            }
        }
    }
}

template <> void
Thread::sleep<SYNC_ADAPTIVE>()
{
    sleep<SYNC_PULSED>();
}

void
Thread::main()
{
    debug(RUN_DEBUG, "main()\n");

    baseTime = util::Time::now();

    while (1) {

        if (isRunning()) {

            switch (getSyncMode()) {

                case SYNC_PERIODIC:   execute<SYNC_PERIODIC>(); break;
                case SYNC_PULSED:     execute<SYNC_PULSED>(); break;
                case SYNC_ADAPTIVE:   execute<SYNC_ADAPTIVE>(); break;
            }
        }

        if (!warp || !isRunning()) {
            
            switch (getSyncMode()) {

                case SYNC_PERIODIC:   sleep<SYNC_PERIODIC>(); break;
                case SYNC_PULSED:     sleep<SYNC_PULSED>(); break;
                case SYNC_ADAPTIVE:   sleep<SYNC_ADAPTIVE>(); break;
            }
        }
        
        // Are we requested to change state?
        if (stateChangeRequest.test()) {

            switchState(newState);
            stateChangeRequest.clear();
            stateChangeRequest.notify_one();

            if (state == EXEC_HALTED) return;
        }

        // Compute the CPU load once in a while
        if (sliceCounter % (32 * slicesPerFrame()) == 0) {

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

    while (newState != state) {

        if (state == EXEC_OFF && newState == EXEC_PAUSED) {

            CoreComponent::powerOn();
            state = EXEC_PAUSED;

        } else if (state == EXEC_OFF && newState == EXEC_RUNNING) {

            CoreComponent::powerOn();
            state = EXEC_PAUSED;

        } else if (state == EXEC_PAUSED && newState == EXEC_OFF) {

            CoreComponent::powerOff();
            state = EXEC_OFF;

        } else if (state == EXEC_PAUSED && newState == EXEC_RUNNING) {

            CoreComponent::run();
            state = EXEC_RUNNING;

        } else if (state == EXEC_RUNNING && newState == EXEC_OFF) {

            state = EXEC_PAUSED;
            CoreComponent::pause();

        } else if (state == EXEC_RUNNING && newState == EXEC_PAUSED) {

            state = EXEC_PAUSED;
            CoreComponent::pause();

        } else if (state == EXEC_RUNNING && newState == EXEC_SUSPENDED) {

            state = EXEC_SUSPENDED;

        } else if (state == EXEC_SUSPENDED && newState == EXEC_RUNNING) {

            state = EXEC_RUNNING;

        } else if (newState == EXEC_HALTED) {

            CoreComponent::halt();
            state = EXEC_HALTED;

        } else {

            // Invalid state transition
            fatalError;
        }

        debug(RUN_DEBUG, "Changed state to %s\n", ExecutionStateEnum::key(state));
    }
}

void
Thread::switchWarp(bool state, u8 source)
{
    assert(source >= 0 && source < 8);

    if (!isEmulatorThread()) suspend();

    u8 old = warp;
    state ? SET_BIT(warp, source) : CLR_BIT(warp, source);

    if (bool(old) != bool(warp)) {
        CoreComponent::warpOnOff(warp);
    }

    if (!isEmulatorThread()) resume();
}

void
Thread::switchTrack(bool state, u8 source)
{
    assert(source >= 0 && source < 8);
    assert(isEmulatorThread() || !isRunning());

    u8 old = track;
    state ? SET_BIT(track, source) : CLR_BIT(track, source);

    if (bool(old) != bool(track)) {
        CoreComponent::trackOnOff(track);
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
Thread::warpOn(isize source)
{
    SUSPENDED switchWarp(true, u8(source));
}

void
Thread::warpOff(isize source)
{
    SUSPENDED switchWarp(false, u8(source));
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
    if (getSyncMode() != SYNC_PERIODIC) {

        trace(TIM_DEBUG, "wakeup: %lld us\n", wakeupClock.restart().asMicroseconds());
        util::Wakeable::wakeUp();
    }
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
