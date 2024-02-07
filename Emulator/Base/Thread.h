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
 * state. 
 *
 * 1. Thread states:
 *
 * The following states are distinguished:
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
 * 2. Suspend / Resume:
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
 * 3. Synchronization:
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
 *   time the thread had been lauchen. After that, it executes all missing
 *   frames or resynchronizes if the number of missing frames is way off.
 *
 * 4. Time slicing:
 *
 * The number of time slices per frame controls the size of a single
 * computation chunk. Per default, the emulator thread computes an entire frame
 * in a single chunk. That is, it computes a frame, sleeps, computes a frame,
 * sleeps, and so on. A frame can be time-sliced to make the emulator more
 * responsive and let it react faster to external events such as joystick
 * movements. For example, if two-time slices are chosen per frame, the thread
 * computes the first half of the current frame, sleeps, computes the second
 * half of the current frame, sleeps, and so on. Note that increasing the
 * number of time slices can increase CPU load and jitter, even in pulsed mode.
 * Jitter may increase because time slices are distributed equally between two
 * wake-up events. Hence, the later chunks will be computed close to the next
 * wake-up event and may, therefore, interfere with the VSYNC event of the host
 * computer.
 *
 * 5. Warp mode:
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

    friend class Emulator;
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

    // Time stamps for calculating wakeup times
    util::Time baseTime;
    util::Time deltaTime;
    util::Time targetTime;

    // Number of time slices that need to be computed
    isize missing = 0;
    
    // Clocks for measuring the CPU load
    util::Clock nonstopClock;
    util::Clock loadClock;

    // The current CPU load in percent
    double cpuLoad = 0.0;

    // Debug clocks
    util::Clock execClock;
    util::Clock wakeupClock;

    
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
    
    // The code to be executed in each iteration (implemented by the subclass)
    virtual void execute() = 0;

    // Target frame rate of this thread (provided by the subclass)
    virtual double refreshRate() const = 0;

    // Number of thread syncs per frame (provided by the subclass)
    virtual isize slicesPerFrame() const = 0;

    // Time span between two wakeup calls (provided by the subclass)
    virtual util::Time wakeupPeriod() const = 0;

    // Computes the time span between two frames
    util::Time frameDuration() const;

    // Computes the time span between two time slices
    util::Time sliceDuration() const;

    // Computes the number of overdue time slices
    isize missingSlices() const;

    // Rectifies an out-of-sync condition by resetting all counters and clocks
    void resync();

    // Executes a single time slice (if one is pending)
    template <SyncMode M> void execute();

    // Suspends the thread until the next time slice is due
    template <SyncMode M> void sleep();

    // The main entry point (called when the thread is created)
    void main();

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

    virtual void readyToGo() = 0;

    //
    // Synchronizing
    //

public:

    // Provides the current sync mode
    virtual SyncMode getSyncMode() const = 0;

    // Awakes the thread if it runs in pulse mode or adaptive mode
    void wakeUp();

private:
    
    // Wait until the thread has terminated
    void join() { if (thread.joinable()) thread.join(); }
};

}
