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
#include "Chrono.h"
#include "Concurrency.h"

namespace vc64 {

/* This class manages the emulator thread that runs side by side with the GUI.
 * The thread exists during the lifetime of the emulator, but may not run the
 * emulator all the time. The exact behavior is controlled by the internal
 * state. The following states are distinguished:
 *
 *        Off: The emulator is turned off
 *     Paused: The emulator is turned on, but not running
 *    Running: The emulator is turned on and running
 *  Suspended: The emulator is paused for a short period of time
 *     Halted: The emulator is shutting down
 *
 *   ---------  powerOn   ---------    run     ---------  suspend   ---------
 *  |   Off   |--------->| Paused  |--------->| Running |--------->|Suspended|
 *  |         |<---------|         |<---------|         |<---------|         |
 *   ---------  powerOff  ---------   pause    ---------   resume   ---------
 *       ^                                         |
 *       |                   powerOff()            |
 *        -----------------------------------------
 *
 *   isPoweredOff          isPaused             isRunning        isSuspended
 *  |-------------||---------------------||--------------------||------------|
 *                 |---------------------------------------------------------|
 *                                       isPoweredOn
 *
 * State changes are triggered by the following functions:
 *
 * Command    | Current   | Next      | Actions on the delegate
 * ------------------------------------------------------------------------
 * powerOn()  | off       | paused    | _powerOn()
 *            | paused    | paused    | none
 *            | running   | running   | none
 *            | suspended | ---       | Error
 * ------------------------------------------------------------------------
 * powerOff() | off       | off       | none
 *            | paused    | off       | _powerOff()
 *            | running   | off       | _powerOff() + _pause()
 *            | suspended | ---       | Error
 * ------------------------------------------------------------------------
 * run()      | off       | ---       | Error
 *            | paused    | running   | _run()
 *            | running   | running   | none
 *            | suspended | ---       | Error
 * ------------------------------------------------------------------------
 * pause()    | off       | off       | none
 *            | paused    | paused    | none
 *            | running   | paused    | _pause()
 *            | suspended | ---       | Error
 * ------------------------------------------------------------------------
 * suspend()  | off       | ---       | Error
 *            | paused    | ---       | Error
 *            | running   | suspended | none
 *            | suspended | suspended | none
 * ------------------------------------------------------------------------
 * resume()   | off       | ---       | Error
 *            | paused    | ---       | Error
 *            | running   | ---       | Error
 *            | suspended | running   | none
 * ------------------------------------------------------------------------
 * halt()     | --        | halted    | _halt()
 *
 * When an instance of the Thread class is created, a new thread is started
 * which executes the thread's main() function. This function executes
 * a loop which periodically calls function execute(). After each iteration,
 * the thread is put to sleep to synchronize timing.
 *
 * The Thread class provides a suspend-resume mechanism for pausing the thread
 * temporarily. This functionality is utilized frequently by the GUI to carry
 * out atomic operations that cannot be performed while the emulator is running.
 * To pause the emulator temporarily, the critical code section can be embedded
 * in a suspend/resume block like this:
 *
 *       suspend();
 *       do something with the internal state;
 *       resume();
 *
 * It it safe to nest multiple suspend/resume blocks, but it is essential
 * that each call to suspend() is followed by a call to resume(). As a result,
 * the critical code section must not be exited in the middle, e.g., by
 * throwing an exception. It is therefore recommended to use the SUSPENDED
 * macro which is exit-safe. It is used in the following way:
 *
 *    {  SUSPENDED
 *
 *       Do something with the internal state;
 *       return or throw an exceptions as you like;
 *    }
 *
 * The Thread class is also responsible for timing synchronization. I.e., it
 * has to ensure that the proper amount of frames are executed per second.
 * Three different synchronization modes are supported:
 *
 * - Periodic:
 *
 *   In periodic mode the thread puts itself to sleep and utilizes a timer to
 *   schedule a wakeup call. In this mode, no further action has to be taken
 *   by the GUI. This method had been the default mode used by earlier versions
 *   VirtualC64.
 *
 * - Pulsed:
 *
 *   In pulsed mode, the thread waits for an external wake-up signal that has
 *   to be sent by the GUI. When the wake-up signal is received, a single frame
 *   is computed. VirtualC64 uses this mode to implement VSYNC.
 *
 * - Adaptive:
 *
 *   In adaptive mode, the thread waits for an external wake-up signal just as
 *   it does in pulsed mode. When the wake-up signal comes in, the thread
 *   computes the number of missing frames based on the current time and the
 *   time the thread had been lauchen. Then it executes all missing frames or
 *   resynchronizes if the number of missing frames is way off. Adaptive mode
 *   has been introduced as a replacement for Pulsed mode.
 *
 * To speed up emulation (e.g., during disk accesses), the emulator may be put
 * into warp mode. In this mode, timing synchronization is disabled causing the
 * emulator to run as fast as possible.
 *
 * Similar to warp mode, the emulator may be put into track mode. This mode is
 * enabled when the GUI debugger is opend and disabled when the debugger is
 * closed. In track mode, several time-consuming tasks are performed that are
 * usually left out. E.g., the CPU tracks all executed instructions and stores
 * the recorded information in a trace buffer.
 */

class Thread : public CoreComponent, util::Wakeable {
    
protected:

    friend class C64;
    
    // The thread object
    std::thread thread;
    
    // The current thread state and a change request
    ExecutionState state = EXEC_OFF;
    ExecutionState newState = EXEC_OFF;
    std::atomic_flag stateChangeRequest {};

    // Warp state and track state
    u8 warp = 0;
    u8 track = 0;

    // Counters
    isize suspendCounter = 0;
    isize sliceCounter = 0;
    isize frameCounter = 0;

    // Debug clocks
    util::Clock execClock;
    util::Clock wakeupClock;

    // Reference time stamp for periodic sync mode
    util::Time targetTime;

    // Reference time stamp for adaptive sync mode
    util::Time baseTime;
    
    // Experimental
    util::Time deltaTime;
    isize missing = 0;
    
    // Clocks for measuring the CPU load
    util::Clock nonstopClock;
    util::Clock loadClock;

    // The current CPU load (%)
    double cpuLoad = 0.0;

    
    //
    // Initializing
    //

public:
    
    Thread();
    ~Thread();
    
    const char *getDescription() const override { return "Thread"; }

    
    //
    // Executing
    //

private:
    
    template <ThreadMode M> void execute();
    template <ThreadMode M> void sleep();

    // Called inside execute when an out-of-sync condition has been detected
    void resync();

    // The main entry point (called when the thread is created)
    void main();

    // Returns the time to elapse between two frames
    util::Time frameDuration() const;

    // Returns the time to elapse between two slices
    util::Time sliceDuration() const;

    // Returns the number of pending emulation chunks
    isize missingSlices() const;

    // The code to be executed in each iteration (implemented by the subclass)
    virtual void execute() = 0;

    // Target frame rate of this thread (provided by the subclass)
    virtual double refreshRate() const = 0;

    // Number of thread syncs per frame (provided by the subclass)
    virtual isize slicesPerFrame() const = 0;

    // Time span between two wakeup calls (provided by the subclass)
    virtual util::Time wakeupPeriod() const = 0;

public:

    // Returns true if this functions is called from within the emulator thread
    bool isEmulatorThread() { return std::this_thread::get_id() == thread.get_id(); }
    
    // Performs a state change
    void switchState(ExecutionState newState);
    void switchWarp(bool state, u8 source = 0);
    void switchTrack(bool state, u8 source = 0);


    //
    // Analyzing
    //

public:
    
    double getCpuLoad() { return cpuLoad; }
    
    
    //
    // Managing states
    //
    
public:
    
    bool isPoweredOn() const override { return state != EXEC_OFF; }
    bool isPoweredOff() const override { return state == EXEC_OFF; }
    bool isPaused() const override { return state == EXEC_PAUSED; }
    bool isRunning() const override { return state == EXEC_RUNNING; }
    bool isSuspended() const override { return state == EXEC_SUSPENDED; }
    bool isHalted() const override { return state == EXEC_HALTED; }

    void suspend() override;
    void resume() override;

    void powerOn();
    void powerOff();
    void run() throws;
    void pause();
    void halt();

    bool isWarping() const { return warp != 0; }
    void warpOn(isize source = 0);
    void warpOff(isize source = 0);

    bool isTracking() const { return track != 0; }
    void trackOn(isize source = 0);
    void trackOff(isize source = 0);

protected:

    // Initiates a state change
    void changeStateTo(ExecutionState requestedState);

    
    //
    // Synchronizing
    //

public:

    // Provides the current sync mode
    virtual ThreadMode getThreadMode() const = 0;

    // Awakes the thread if it runs in pulse mode
    void wakeUp();

private:
    
    // Wait until the thread has terminated
    void join() { if (thread.joinable()) thread.join(); }
};

struct AutoResume {

    CoreComponent *c;
    AutoResume(CoreComponent *c) : c(c) { c->suspend(); }
    ~AutoResume() { c->resume(); }
};

#define SUSPENDED AutoResume _ar(this);

}
