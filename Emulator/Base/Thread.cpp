// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
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

template <> void
Thread::execute<THREAD_PERIODIC>()
{
    loadClock.go();
    execute();
    loadClock.stop();
}

template <> void
Thread::execute<THREAD_PULSED>()
{
    loadClock.go();
    execute();
    loadClock.stop();
}

template <> void
Thread::execute<THREAD_ADAPTIVE>()
{
    loadClock.go();

    // Get the number of missing frames
    i64 missing = warp ? 1 : missingFrames(baseTime);

    // Resync if necessary
    if (missing < -5 || missing > 5) {

        debug(RUN_DEBUG, "Adaptive sync: Resyncing %lld frames\n", missing);
        baseTime += util::Time(missing * 1000000000LL / i64(refreshRate()));
        missing = 0;
    }

    // Compute all missing frames
    for (isize i = 0; i < missing; i++) execute();

    loadClock.stop();
}

template <> void
Thread::sleep<THREAD_PERIODIC>()
{
    auto now = util::Time::now();

    // Only proceed if we're not running in warp mode
    if (warp) return;

    // Check if we're running too slow...
    if (now > targetTime) {

        // Check if we're completely out of sync...
        if ((now - targetTime).asMilliseconds() > 200) {

            warn("Emulation is way too slow: %f\n",(now - targetTime).asSeconds());

            // Restart the sync timer
            targetTime = util::Time::now();
        }
    }

    // Check if we're running too fast...
    if (now < targetTime) {

        // Check if we're completely out of sync...
        if ((targetTime - now).asMilliseconds() > 200) {

            warn("Emulation is way too slow: %f\n",(targetTime - now).asSeconds());

            // Restart the sync timer
            targetTime = util::Time::now();
        }
    }

    // Sleep for a while
    targetTime += util::Time(i64(1000000000.0 / refreshRate()));
    targetTime.sleepUntil();
}

template <> void
Thread::sleep<THREAD_PULSED>()
{
    // Set a timeout to prevent the thread from stalling
    auto timeout = util::Time(i64(2000000000.0 / refreshRate()));

    // Wait for the next pulse
    if (!warp) waitForWakeUp(timeout);
}

template <> void
Thread::sleep<THREAD_ADAPTIVE>()
{
    // Set a timeout to prevent the thread from stalling
    auto timeout = util::Time(i64(2000000000.0 / refreshRate()));

    // Wait for the next pulse
    if (!warp) waitForWakeUp(timeout);
}

void
Thread::main()
{
    debug(RUN_DEBUG, "main()\n");

    baseTime = util::Time::now();

    while (++loopCounter) {

        if (isRunning()) {

            switch (getThreadMode()) {

                case THREAD_PERIODIC:   execute<THREAD_PERIODIC>(); break;
                case THREAD_PULSED:     execute<THREAD_PULSED>(); break;
                case THREAD_ADAPTIVE:   execute<THREAD_ADAPTIVE>(); break;
            }
        }

        if (!warp || !isRunning()) {
            
            switch (getThreadMode()) {

                case THREAD_PERIODIC:   sleep<THREAD_PERIODIC>(); break;
                case THREAD_PULSED:     sleep<THREAD_PULSED>(); break;
                case THREAD_ADAPTIVE:   sleep<THREAD_ADAPTIVE>(); break;
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
        if (loopCounter % 32 == 0) {
            
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
        isReady();
        
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
    if (getThreadMode() != THREAD_PERIODIC) util::Wakeable::wakeUp();
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
