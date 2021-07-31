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
    auto now = util::Time::now();
    
    // Check if we're running too slow...
    if (now > targetTime) {
        
        // Check if we're completely out of sync...
        if ((now - targetTime).asMilliseconds() > 2000) {
            
            std::cout << "The emulator is way too slow: ";
            std::cout << (now - targetTime).asSeconds() << std::endl;
            restartSyncTimer();
            return;
        }
    }
    
    // Check if we're running too fast...
    if (now < targetTime) {
        
        // Check if we're completely out of sync...
        if ((targetTime - now).asMilliseconds() > 2000) {
            
            std::cout << "The emulator is way too fast: ";
            std::cout << (targetTime - now).asSeconds() << std::endl;
            restartSyncTimer();
            return;
        }
    }
    
    // Call the execution function
    delegate.threadExecute();
    
    // Sleep for a while
    // std::cout << "Sleeping..." << targetTime.asMilliseconds() << std::endl;
    targetTime += delay;
    targetTime.sleepUntil();
}

template <> void
Thread::execute <ThreadMode::Pulsed> ()
{
    // Call the execution function
    delegate.threadExecute();
    
    // Wait for the next pulse
    waitForCondition();
}

void
Thread::main()
{
    while (1) {
                
        if (isRunning()) {
            
            switch (mode) {
                case ThreadMode::Periodic: execute<ThreadMode::Periodic>(); break;
                case ThreadMode::Pulsed: execute<ThreadMode::Pulsed> (); break;
            }
        }
        
        // Are we requested to change state?
        while (newState != state) {
            
            if (state == THREAD_OFF && newState == THREAD_PAUSED) {
                
                delegate.threadPowerOn();
                state = newState;
                break;
            }

            if (state == THREAD_OFF && newState == THREAD_RUNNING) {
                
                delegate.threadPowerOn();
                delegate.threadRun();
                state = newState;
                break;
            }

            if (state == THREAD_PAUSED && newState == THREAD_OFF) {
                
                delegate.threadPowerOff();
                state = newState;
                break;
            }

            if (state == THREAD_PAUSED && newState == THREAD_RUNNING) {
                
                delegate.threadRun();
                state = newState;
                break;
            }

            if (state == THREAD_RUNNING && newState == THREAD_OFF) {
                
                delegate.threadPause();
                delegate.threadPowerOff();
                state = newState;
                break;
            }

            if (state == THREAD_RUNNING && newState == THREAD_PAUSED) {
                
                delegate.threadPause();
                state = newState;
                break;
            }
            
            if (newState == THREAD_TERMINATED) {
                
                delegate.threadHalt();
                state = newState;
                return;
            }
            
            // Invalid state transition
            assert(false);
            break;
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
    restartSyncTimer();
}

void
Thread::powerOn()
{
    debug(RUN_DEBUG, "powerOn()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());

    // Never reenter this function
    assert(!entered); entered = true;

    if (isPoweredOff() && delegate.readyToPowerOn()) {
        
        // Request a state change and wait until the new state has been reached
        changeStateTo(THREAD_PAUSED);
    }
    entered = false;
}

void
Thread::powerOff()
{
    debug(RUN_DEBUG, "powerOff()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());

    // Never reenter this function
    assert(!entered); entered = true;
    
    if (!isPoweredOff()) {
                
        // Request a state change and wait until the new state has been reached
        changeStateTo(THREAD_OFF);
    }
    entered = false;
}

void
Thread::run()
{
    debug(RUN_DEBUG, "run()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());

    // Never reenter this function
    assert(!entered); entered = true;
    
    if (!isRunning() && delegate.readyToPowerOn()) {
        
        // Request a state change and wait until the new state has been reached
        changeStateTo(THREAD_RUNNING);
    }
    entered = false;
}

void
Thread::pause()
{
    debug(RUN_DEBUG, "pause()\n");

    // Never call this function inside the emulator thread
    assert(!isEmulatorThread());

    // Never reenter this function
    assert(!entered); entered = true;
    
    if (isRunning()) {
                
        // Request a state change and wait until the new state has been reached
        changeStateTo(THREAD_PAUSED);
    }
    entered = false;
}

void
Thread::changeStateTo(ThreadEmuState requestedState)
{
    newState = requestedState;
    
    // Wait until the state has changed
    // signalCondition();
    while (state != requestedState) { };
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
