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

Thread::Thread(ThreadDelegate &d) : delegate(d)
{
    restartSyncTimer();
    
    // Start the thread and enter the main function
    thread = std::thread(&Thread::main, this);
}

Thread::~Thread()
{
    // Wait until the thread has terminated
    join();
}

template <> void
Thread::execute <ThreadMode::Periodic> ()
{
    // Call the execution function
    loadClock.go();
    delegate.threadExecute();
    loadClock.stop();
}

template <> void
Thread::execute <ThreadMode::Pulsed> ()
{
    // Call the execution function
    loadClock.go();
    delegate.threadExecute();
    loadClock.stop();
    
}

template <> void
Thread::sleep <ThreadMode::Periodic> ()
{
    auto now = util::Time::now();

    // Only proceed if we're not running in warp mode
    if (warp) return;
        
    // Check if we're running too slow...
    if (now > targetTime) {
        
        // Check if we're completely out of sync...
        if ((now - targetTime).asMilliseconds() > 200) {
            
            warn("Emulation is way too slow: %f\n",(now - targetTime).asSeconds());
            restartSyncTimer();
        }
    }
    
    // Check if we're running too fast...
    if (now < targetTime) {
        
        // Check if we're completely out of sync...
        if ((targetTime - now).asMilliseconds() > 200) {
            
            warn("Emulation is way too slow: %f\n",(targetTime - now).asSeconds());
            restartSyncTimer();
        }
    }
        
    // Sleep for a while
    // std::cout << "Sleeping... " << targetTime.asMilliseconds() << std::endl;
    // std::cout << "Delay = " << delay.asNanoseconds() << std::endl;
    targetTime += delay;
    targetTime.sleepUntil();
}

template <> void
Thread::sleep <ThreadMode::Pulsed> ()
{
    // Wait for the next pulse
    if (!warp) waitForCondition();
}

void
Thread::main()
{
    debug(RUN_DEBUG, "main()\n");
          
    while (++loops) {
           
        if (isRunning()) {
                        
            switch (mode) {
                case ThreadMode::Periodic: execute<ThreadMode::Periodic>(); break;
                case ThreadMode::Pulsed: execute<ThreadMode::Pulsed>(); break;
            }
        }
        
        if (!warp || isPaused()) {

            switch (mode) {
                case ThreadMode::Periodic: sleep<ThreadMode::Periodic>(); break;
                case ThreadMode::Pulsed: sleep<ThreadMode::Pulsed>(); break;
            }
        }
        
        // Are we requested to enter or exit warp mode?
        while (newWarp != warp) {
            
            newWarp ? delegate.threadWarpOn() : delegate.threadWarpOff();
            warp = newWarp;
            break;
        }
        
        // Are we requested to change state?
        while (newState != state) {
            
            if (state == EXEC_OFF && newState == EXEC_PAUSED) {
                
                delegate.threadPowerOn();
                state = newState;
                break;
            }

            if (state == EXEC_OFF && newState == EXEC_RUNNING) {
                
                delegate.threadPowerOn();
                delegate.threadRun();
                state = newState;
                break;
            }

            if (state == EXEC_PAUSED && newState == EXEC_OFF) {
                
                delegate.threadPowerOff();
                state = newState;
                break;
            }

            if (state == EXEC_PAUSED && newState == EXEC_RUNNING) {
                
                delegate.threadRun();
                state = newState;
                break;
            }

            if (state == EXEC_RUNNING && newState == EXEC_OFF) {
                
                delegate.threadPause();
                delegate.threadPowerOff();
                state = newState;
                break;
            }

            if (state == EXEC_RUNNING && newState == EXEC_PAUSED) {
                
                delegate.threadPause();
                state = newState;
                break;
            }
            
            if (newState == EXEC_TERMINATED) {
                
                delegate.threadHalt();
                state = newState;
                return;
            }
            
            // Invalid state transition
            assert(false);
            break;
        }
        
        // Compute the CPU load once in a while
        if (loops % 32 == 0) {
            
            auto used  = loadClock.getElapsedTime().asSeconds();
            auto total = nonstopClock.getElapsedTime().asSeconds();
            
            cpuLoad = used / total;
            
            loadClock.restart();
            loadClock.stop();
            nonstopClock.restart();
            
            // printf("CPU load = %f\n", cpuLoad);
        }
    }
}

void
Thread::setSyncDelay(util::Time newDelay)
{    
    delay = newDelay;
}

void
Thread::setMode(ThreadMode newMode)
{
    if (mode == newMode) return;
    
    mode = newMode;
}

void
Thread::setWarpLock(bool value)
{
    warpLock = value;
}

void
Thread::powerOn(bool blocking)
{
    debug(RUN_DEBUG, "powerOn()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());

    // Never reenter this function
    assert(!entered); entered = true;

    if (isPoweredOff() && delegate.readyToPowerOn()) {
        
        // Request a state change and wait until the new state has been reached
        changeStateTo(EXEC_PAUSED, blocking);
    }
    entered = false;
}

void
Thread::powerOff(bool blocking)
{
    debug(RUN_DEBUG, "powerOff()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());

    // Never reenter this function
    assert(!entered); entered = true;
    
    if (!isPoweredOff()) {
                
        // Request a state change and wait until the new state has been reached
        changeStateTo(EXEC_OFF, blocking);
    }
    entered = false;
}

void
Thread::run(bool blocking)
{
    debug(RUN_DEBUG, "run()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());

    // Never reenter this function
    assert(!entered); entered = true;
    
    if (!isRunning() && delegate.readyToPowerOn()) {
        
        // Request a state change and wait until the new state has been reached
        changeStateTo(EXEC_RUNNING, blocking);
    }
    entered = false;
}

void
Thread::pause(bool blocking)
{
    debug(RUN_DEBUG, "pause()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());

    // Never reenter this function
    assert(!entered); entered = true;
    
    if (isRunning()) {
                
        // Request a state change and wait until the new state has been reached
        changeStateTo(EXEC_PAUSED, blocking);
    }
    entered = false;
}

void
Thread::halt(bool blocking)
{
    changeStateTo(EXEC_TERMINATED, blocking);
}

void
Thread::warpOn(bool blocking)
{
    if (!warpLock) changeWarpTo(true, blocking);
}

void
Thread::warpOff(bool blocking)
{
    if (!warpLock) changeWarpTo(false, blocking);
}

void
Thread::changeStateTo(ExecutionState requestedState, bool blocking)
{
    newState = requestedState;
    if (blocking) while (state != newState) { };
}

void
Thread::changeWarpTo(bool value, bool blocking)
{
    newWarp = value;
    if (blocking) while (warp != newWarp) { };
}

void
Thread::waitForCondition()
{
    std::unique_lock<std::mutex> lock(condMutex);
    condFlag = false;
    cond.wait_for(lock,
                  std::chrono::seconds(1000),
                  [this]() { return condFlag; } );
}

void
Thread::signalCondition()
{
    std::lock_guard<std::mutex> lock(condMutex);
    condFlag = true;
    cond.notify_one();
}

void
Thread::pulse()
{
    if (mode == ThreadMode::Pulsed) {
        signalCondition();
    }
}

void
Thread::restartSyncTimer()
{
    targetTime = util::Time::now();
}
