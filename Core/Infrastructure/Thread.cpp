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
    assert(!isInitialized());

    // Start the thread and enter the main function
    thread = std::thread(&Thread::runLoop, this);

    // Wait until the thread is fully initialized
    initLatch.wait();

    assert(isLaunched());
    assert(isInitialized());
}

void
Thread::resync()
{
    resyncs++;
    baseTime = utl::Time::now();
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

        lock.lock();
        loadClock.go();

        try {
            
            // Execute all missing frames
            for (isize i = 0; i < missing; i++, frameCounter++) {
                
                // Execute a single frame
                computeFrame();
            }
            
        } catch (StateChangeException &exc) {
            
            // Serve a state change request
            switchState((ExecState)exc.data);
        }
        
        loadClock.stop();
        lock.unlock();

    } else {

        // The emulator is out of sync
        if (missing > 0) {
            loginfo(RUN_DEBUG, "Emulation is way too slow (%ld frames behind)\n", missing);
        } else {
            loginfo(RUN_DEBUG, "Emulation is way too fast (%ld time slices ahead)\n", -missing);
        }

        resync();
    }
}

void
Thread::sleep()
{
    // Wait for a pending suspension to finish
    suspensionLock.lock();
    suspensionLock.unlock();

    // Don't sleep if the emulator is running in warp mode
    if (warp && isRunning()) return;
    /*
    // Don't sleep if the emulator is running in warp mode and no suspension is pending
    if (warp && isRunning() && suspensionLock.tryLock()) {
        
        suspensionLock.unlock();
        return;
    }
    */

    // Set a timeout to prevent the thread from stalling
    auto timeout = utl::Time::milliseconds(50);

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
    initialize();

    while (state != ExecState::HALTED) {

        // Prepare for the next frame
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
Thread::switchState(ExecState newState)
{
    assert(isEmulatorThread() || !isRunning());

    auto invalid = [&]() {
        
        assert(false);
        fatal("Invalid state transition: %s -> %s\n",
              ExecStateEnum::key(state), ExecStateEnum::key(newState));
    };

    loginfo(RUN_DEBUG,
          "switchState: %s -> %s\n",
          ExecStateEnum::key(state), ExecStateEnum::key(newState));

    if (!isLaunched()) {

        throw std::runtime_error(string("The emulator thread hasn't been lauchend yet. "
                                        "Missing call to launch()."));
    }

    while (state != newState) {

        switch (newState) {

            case ExecState::OFF:

                switch (state) {

                    case ExecState::PAUSED:     state = ExecState::OFF; _powerOff(); break;
                    case ExecState::RUNNING:    state = ExecState::PAUSED; _pause(); break;

                    default:
                        invalid();
                }
                break;

            case ExecState::PAUSED:

                switch (state) {

                    case ExecState::OFF:        state = ExecState::PAUSED; _powerOn(); break;
                    case ExecState::RUNNING:    state = ExecState::PAUSED; _pause(); break;

                    default:
                        invalid();
                }
                break;

            case ExecState::RUNNING:

                switch (state) {

                    case ExecState::OFF:        state = ExecState::PAUSED; _powerOn(); break;
                    case ExecState::PAUSED:     state = ExecState::RUNNING; _run(); break;

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

    loginfo(RUN_DEBUG, "switchState: %s\n", ExecStateEnum::key(state));
    assert(state == newState);
}

void
Thread::powerOn()
{
    loginfo(RUN_DEBUG, "powerOn()\n");

    if (isPoweredOff()) {

        switchState(ExecState::PAUSED);
    }
}

void
Thread::powerOff()
{
    loginfo(RUN_DEBUG, "powerOff()\n");

    if (!isPoweredOff()) {

        switchState(ExecState::OFF);
    }
}

void
Thread::run()
{
    loginfo(RUN_DEBUG, "run()\n");

    if (!isRunning()) {

        // Throw an exception if the emulator is not ready to run
        isReady();

        switchState(ExecState::RUNNING);
    }
}

void
Thread::pause()
{
    loginfo(RUN_DEBUG, "pause()\n");

    if (isRunning()) {

        switchState(ExecState::PAUSED);
    }
}

void
Thread::halt()
{
    loginfo(RUN_DEBUG, "halt()\n");

    if (state != ExecState::HALTED) {

        switchState(ExecState::HALTED);
    }
}

void
Thread::warpOn(isize source)
{
    assert(source < 7);

    if (!GET_BIT(warp, source)) {

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

        auto old = track;
        CLR_BIT(track, source);
        if (!!old != !!track) _trackOff();
    }
}

void
Thread::wakeUp()
{
    // logdebug(RUN_DEBUG, "wakeup: %lld us\n", wakeupClock.restart().asMicroseconds());
    Wakeable::wakeUp();
}

void
Thread::suspend() const
{
    loginfo(RUN_DEBUG, "Suspending (%ld)...\n", suspendCounter);
    assert(isUserThread());

    if (suspendCounter++ == 0) {
        
        suspensionLock.lock();
        lock.lock();
    }
}

void
Thread::resume() const
{
    loginfo(RUN_DEBUG, "Resuming (%ld)...\n", suspendCounter);
    assert(isUserThread());

    if (suspendCounter <= 0) {
        
        fatal("resume() called with no call to suspend()\n");
        
    } else if (--suspendCounter == 0) {
        
        suspensionLock.unlock();
        lock.unlock();
    }
}

}
