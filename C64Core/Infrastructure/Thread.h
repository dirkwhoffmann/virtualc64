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

#pragma once

#include "ThreadTypes.h"
#include "CoreComponent.h"
// #include "utl/chrono.h"
// #include "utl/concurrency.h"
#include "utl/abilities/Wakeable.h"
#include <thread>
#include <latch>

namespace vc64 {

/** Requests a state change from within the emulator.
 *  This exception is thrown inside the emulator core when the CPU stops
 *  execution in the middle of frame. This happens when a breakpoint or
 *  watchpoint is hit, or when the CPU halts due to the execution of a jamming
 *  instruction */
typedef AppException StateChangeException;

/** Implements the emulator's state model.
 *  This class is one of the base classes of the Emulator class and provides
 *  the basic functionality to manage the execution state. It provides functions
 *  to launch the emulator thread, to query it's current state, and to switch
 *  to another state. */
class Thread : public CoreObject, public utl::Wakeable {

protected:

    // The thread object
    std::thread thread;
    
    // The current thread state
    ExecState state = ExecState::OFF;

    // A latch controlling the launch procedure
    std::latch initLatch {1};

    // Synchronization mutex
    mutable utl::ReentrantMutex lock;
    mutable utl::ReentrantMutex suspensionLock;
    
    // Warp and track state
    u8 warp = 0;
    u8 track = 0;

    // Counters
    mutable isize suspendCounter = 0;
    isize frameCounter = 0;
    isize statsCounter = 0;

    // Time stamps
    utl::Time baseTime;
    
    // Clocks for measuring the CPU load
    utl::Clock nonstopClock;
    utl::Clock loadClock;

    // Statistical information (CPU load, frames per second, thread resyncs)
    double cpuLoad = 0.0;
    double fps = 0.0;
    isize resyncs = 0;

    // Debug clocks
    utl::Clock wakeupClock;

    
    //
    // Initializing
    //

public:
    
    Thread() { };
    ~Thread() { };
    
    const char *objectName() const override { return "Thread"; }

    // Checks the launch state
    bool isLaunched() const { return thread.joinable(); }

protected:

    // Launches the emulator thread
    void launch();


    //
    // Executing
    //
    
public:

    // Returns true if this functions is called inside or outside the emulator thread
    bool isEmulatorThread() const { return std::this_thread::get_id() == thread.get_id(); }
    bool isUserThread() const { return std::this_thread::get_id() != thread.get_id(); }
    
    // Performs a state change
    void switchState(ExecState newState);

private:

    // Initializes the emulator (implemented by the subclass)
    virtual void initialize() = 0;

    // Updates the emulator state (implemented by the subclass)
    virtual void update() = 0;

    // Computes the number of overdue frames (provided by the subclass)
    virtual isize missingFrames() const = 0;

    // The code to be executed in each iteration (implemented by the subclass)
    virtual void computeFrame() = 0;

    // Rectifies an out-of-sync condition. Resets all counters and clocks.
    void resync();

    /** The thread's main entry point.
     *
     *  This function is called when the thread is created.
     */
    void runLoop();

    // Computes all missing frames
    void execute();

    // Suspends the thread till the next wakeup pulse
    void sleep();


    //
    // Analyzing
    //

private:

    void computeStats();


    //
    // Managing states
    //

public:

    /** Returns the current suspension state.
     */
    bool isSuspended() const { return suspendCounter > 0; }
    
    /** Suspends the thread.
     *  The thread is temporarily suspended
     */
    void suspend() const;

    /** Resumes the thread.
     *  The thread is put back in running state
     */
    void resume() const;

    bool isInitialized() const { return initLatch.try_wait(); }
    bool isPoweredOn() const { return state != ExecState::OFF; }
    bool isPoweredOff() const { return state == ExecState::OFF; }
    bool isPaused() const { return state == ExecState::PAUSED; }
    bool isRunning() const { return state == ExecState::RUNNING; }
    bool isHalted() const { return state == ExecState::HALTED; }
    bool isWarping() const { return warp != 0; }
    bool isTracking() const { return track != 0; }

    void powerOn();
    void powerOff();
    void run();
    void pause();
    void halt();

    void warpOn(isize source = 0);
    void warpOff(isize source = 0);
    void trackOn(isize source = 0);
    void trackOff(isize source = 0);

    // Signals a state transition
    virtual void _powerOn() = 0;
    virtual void _powerOff() = 0;
    virtual void _pause() = 0;
    virtual void _run() = 0;
    virtual void _halt() = 0;
    virtual void _warpOn() = 0;
    virtual void _warpOff() = 0;
    virtual void _trackOn() = 0;
    virtual void _trackOff() = 0;

private:

    // Returns if the emulator is ready to runs, throws an exception otherwise
    virtual void isReady() = 0;


    //
    // Synchronizing
    //

public:

    /** Awakes the thread.
     *  Wakes up the emulator thread.
     */
    void wakeUp();
    
    // Wait until the thread has terminated
    void join() { if (thread.joinable()) thread.join(); }
};

}
