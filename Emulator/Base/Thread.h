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
#include "Wakeable.h"

namespace vc64 {

/* This class manages the emulator thread that runs side by side with the GUI.
 * The thread exists during the lifetime of the emulator, but may not run the
 * emulator all the time. The exact behavior is controlled by the internal
 * state. 
 *
 * The following states are distinguished:
 *
 *        Off: The emulator is turned off
 *     Paused: The emulator is turned on but not running
 *    Running: The emulator is turned on and running
 *  Suspended: The emulator is paused for a short period of time
 *     Halted: The emulator is shutting down
 *
 *                                                          halt()    ---------
 *       -------------------------------------------   ...---------->| Halted  |
 *      |                   run()                   |                 ---------
 *      |                                           v
 *  ---------  powerOn()  ---------    run()    ---------  suspend()  ---------
 * |   Off   |---------->| Paused  |---------->| Running |---------->|Suspended|
 * |         |<----------|         |<----------|         |<----------|         |
 *  ---------  powerOff() ---------   pause()   ---------  resume()   ---------
 *      ^                                           |
 *      |                powerOff()                 |
 *       -------------------------------------------
 *
 *  isPoweredOff()       isPaused()            isRunning()       isSuspended()
 * |--------------||--------------------||--------------------||---------------|
 *                 |-----------------------------------------------------------|
 *                                       isPoweredOn()
 *
 * When an instance of the Thread class is created, a new thread is started
 * which executes the thread's main() function. This function executes
 * a loop which periodically calls function execute(). After each iteration,
 * the thread is put to sleep to synchronize timing.
 *
 * Suspend / Resume:
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
 * Synchronization:
 *
 * The Thread class is responsible for timing synchronization. I.e., it has to
 * ensure that the proper amount of frames are executed per second. To
 * synchronize timing, the thread waits for an external wake-up signal. When
 * the wake-up signal comes in (by calling wakeup()), the thread computes the
 * computes computes all missing frames. If the number of missing frames
 * exceeds a threshold, the thread resynchronizes itself by calling resync().
 *
 * Warp mode:
 *
 * To speed up emulation (e.g., during disk accesses), the emulator may be put
 * into warp mode. In this mode, timing synchronization is disabled causing the
 * emulator to run as fast as possible.
 *
 * DEPRECATED:
 * Similar to warp mode, the emulator may be put into track mode. This mode is
 * enabled when the GUI debugger is opend and disabled when the debugger is
 * closed. In track mode, several time-consuming tasks are performed that are
 * usually left out. E.g., the CPU tracks all executed instructions and stores
 * the recorded information in a trace buffer.
 */

class Thread : public CoreObject, public Suspendable, Wakeable {

protected:

    // The thread object
    std::thread thread;
    
    // The current thread state and a change request
    EmulatorState state = STATE_OFF;
    EmulatorState newState = STATE_OFF;
    std::atomic_flag stateChangeRequest {};

    // Warp and track state
    u8 warp = 0, oldWarp = 0;
    u8 track = 0, oldTrack = 0;

    // Counters
    isize suspendCounter = 0;
    isize frameCounter = 0;

    // Time stamps
    util::Time baseTime;
    
    // Clocks for measuring the CPU load
    util::Clock nonstopClock;
    util::Clock loadClock;

    // The current CPU load in percent
    double cpuLoad = 0.0;

    // Debug clocks
    util::Clock wakeupClock;

    
    //
    // Initializing
    //

public:
    
    Thread();
    ~Thread();
    
    const char *getDescription() const override { return "Thread"; }

protected:

    // Launches the emulator thread
    void launch();


    //
    // Executing
    //
    
public:

    // Returns true if this functions is called from within the emulator thread
    bool isEmulatorThread() { return std::this_thread::get_id() == thread.get_id(); }

    // Performs a state change
    void switchState(EmulatorState newState);

private:

    // Returns the current warp and track status (provided by the subclass)
    virtual bool shouldWarp() { return false; }
    virtual bool shouldTrack() { return false; }

    // Updates the current warp and track state
    void updateWarp();
    void updateTrack();

    // Computes the number of overdue frames (provided by the subclass)
    virtual isize missingFrames() const = 0;

    // Target frame rate of this thread (provided by the subclass)
    virtual double refreshRate() const = 0;

    // The code to be executed in each iteration (implemented by the subclass)
    virtual void computeFrame() = 0;

    // Rectifies an out-of-sync condition by resetting all counters and clocks
    void resync();

    // The main entry point (called when the thread is created)
    void main();

    // Computes all missing frames
    void execute();

    // Suspends the thread till the next wakeup pulse
    void sleep();


    //
    // Analyzing
    //

public:
    
    double getCpuLoad() { return computeCpuLoad(); }

private:

    double computeCpuLoad();


    //
    // Managing states
    //
    
public:

    void suspend() override;
    void resume() override;

    bool isPoweredOn() const { return state != STATE_OFF; }
    bool isPoweredOff() const { return state == STATE_OFF; }
    bool isPaused() const { return state == STATE_PAUSED; }
    bool isRunning() const { return state == STATE_RUNNING; }
    bool isSuspended() const { return state == STATE_SUSPENDED; }
    bool isHalted() const { return state == STATE_HALTED; }
    bool isWarping() const { return warp != 0; }
    bool isTracking() const { return track != 0; }

    void powerOn();
    void powerOff();
    void run() throws;
    void pause();
    void halt();
    void warpOn(isize source = 0);
    void warpOff(isize source = 0);
    void trackOn(isize source = 0);
    void trackOff(isize source = 0);

private:

    // Initiates a state change
    void changeStateTo(EmulatorState requestedState);

    // Signals a state change
    virtual void stateChange(ThreadTransition) = 0;

    // Returns if the emulator is ready to runs, throws an exception otherwise
    virtual void readyToGo() throws = 0;


    //
    // Synchronizing
    //

public:

    // Awakes the thread
    void wakeUp();

private:
    
    // Wait until the thread has terminated
    void join() { if (thread.joinable()) thread.join(); }
};

}
