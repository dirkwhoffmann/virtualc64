// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "ThreadTypes.h"
#include "C64Component.h"
#include "Chrono.h"
#include <thread>

/* The Thread class manages the emulator thread that runs side by side to
 * the graphical user interface. The thread exists during the lifetime of
 * the emulator instance, but does not have to be active all the time. The
 * behavior of the thread is controlled by its internal state, which we
 * refer to as the emulator state. Three possible states are distinguished:
 *
 *        Off: The emulator is turned off.
 *     Paused: The emulator is turned on, but not running.
 *    Running: The emulator is turned on and running.
 *
 *          -----------------------------------------------
 *         |                     run()                     |
 *         |                                               V
 *     ---------   powerOn()   ---------     run()     ---------
 *    |   Off   |------------>| Paused  |------------>| Running |
 *    |         |<------------|         |<------------|         |
 *     ---------   powerOff()  ---------    pause()    ---------
 *         ^                                               |
 *         |                   powerOff()                  |
 *          -----------------------------------------------
 *
 *     isPoweredOff()         isPaused()          isRunning()
 * |-------------------||-------------------||-------------------|
 *                      |----------------------------------------|
 *                                     isPoweredOn()
 *
 * State changes are triggered by the following functions:
 *
 * Command    | Current   | Next      | Actions
 * ------------------------------------------------------------------------
 * powerOn()  | off       | paused    | delegate.threadPowerOn()
 *            | paused    |  paused   | none
 *            | running   | running   | none
 * ------------------------------------------------------------------------
 * powerOff() | off       | off       | none
 *            | paused    | off       | delegate.threadPowerOff()
 *            | running   | off       | pause(), delegate.threadPause()
 * ------------------------------------------------------------------------
 * run()      | off       | running   | powerOn(), delegate.run()
 *            | paused    | running   | delegate.run()
 *            | running   | running   | none
 * ------------------------------------------------------------------------
 * pause()    |  off      | off       | none
 *            | paused    | paused    | none
 *            | running   | paused    | delegate.pause()
 *
 * When an instance of the Thread class has been created, a new thread is
 * started which executes the thread's main() function. This function executes
 * a loop which periodically calls the threadExecute() function of the
 * delegation object. After that, the thread is put to sleep to synchronize
 * timing. Two synchronization modes are supported: Timed or Pulsed. The
 * first mode puts the thread the sleep for a certain amout of time. The
 * second mode puts the thread to sleep and waits for an external pulse
 * signal to continue.
 *
 */

class ThreadDelegate {
    
public:
    
    virtual ~ThreadDelegate() { };
    
    virtual bool readyToPowerOn() = 0;
    
    virtual void threadPowerOff() = 0;
    virtual void threadPowerOn() = 0;
    virtual void threadRun() = 0;
    virtual void threadPause() = 0;
    virtual void threadExecute() = 0;
    virtual void threadHalt() = 0;
};

class Thread : public C64Object {
    
    // The actual thread
    std::thread thread;

    // The thread delegate
    ThreadDelegate &delegate;

    // The selected timing synchronization mode
    volatile ThreadMode mode = ThreadMode::Periodic;
    
    // The current thread state
    volatile ThreadState state = THREAD_OFF;
    
    // A request to change the thread state
    volatile ThreadState newState = THREAD_OFF;
    
    // Variables needed to implement "pulsed" mode
    std::mutex condMutex;
    std::condition_variable cond;
    bool condFlag = false;

    // Variables needed to implement "periodic" mode
    util::Time delay = util::Time(1000000000);
    util::Time targetTime;
    
    // Variable used for guarding non-reentrant functions
    bool entered = false;
    
    
    //
    // Initializing
    //

public:
    
    Thread(ThreadDelegate &d);
    ~Thread();
    
    // Returns true if this functions is called from within the emulator thread
    bool isEmulatorThread() { return std::this_thread::get_id() == thread.get_id(); }

    
    //
    // Executing
    //

private:
    
    template <ThreadMode M> void execute();
    void main();

    
    //
    // Configuring
    //

public:
    
    void setSyncDelay(util::Time newDelay);
    void setMode(ThreadMode newMode);

    
    //
    // Managing states
    //
    
public:
    
    bool isPoweredOff() const { return state == THREAD_OFF; }
    bool isPoweredOn() const { return state != THREAD_OFF; }
    bool isPaused() const { return state == THREAD_PAUSED; }
    bool isRunning() const { return state == THREAD_RUNNING; }

    void powerOn();
    void powerOff();
    void run();
    void pause();

private:
    
    void changeStateTo(ThreadState requestedState);
    
    void waitForCondition();
    void signalCondition();
    
    
    //
    // Syncing the thread
    //

public:
    
    // Awakes the thread if it runs in sync mode
    void pulse();

private:
    
    // Resynchonizes a thread that got out-of-sync
    void restartSyncTimer();

    // Wait until the thread has terminated
    void join() { if (thread.joinable()) thread.join(); }

    
    
    
};
