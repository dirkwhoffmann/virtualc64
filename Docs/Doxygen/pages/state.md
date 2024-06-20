# State model

## Overview

After lauching, the emulator runs as a single thread alongside the GUI. The thread exists during the lifetime of the application, but may not compute new frames all the time. The exact behavior is controlled by the internal state. 
 
 The following states are distinguishedd:
 
 - **Uninit:** The emulator has just been created
 - **Off:** The emulator is turned off
 - **Paused:** The emulator is turned on but not running
 - **Running:** The emulator is turned on and running
 - **Suspended:** The emulator is paused for a short period of time
 - **Halted:** The emulator is shutting down
 
 @image html state.png
 
 The thread executes an infinite loop which periodically calls function vc64::Thread::execute. After each iteration, the thread is put to sleep to synchronize timing.
 
 ## Suspend / Resume
 
 The Thread class provides a suspend-resume mechanism for pausing the thread temporarily. This functionality is utilized frequently by the GUI to carry out atomic operations that cannot be performed while the emulator is running. To pause the emulator temporarily, the critical code section can be embedded in a suspend/resume block like this:

 ```{.cpp}
 	suspend();

    // Modify the internal state

    resume();
 ```

 It it safe to nest multiple suspend/resume blocks, but it is essential that each call to [suspend](@ref vc64::Thread::suspend) is followed by a call to Thread class [resume](@ref vc64::Thread::resume). As a result, the critical code section must not be exited in the middle, e.g., by throwing an exception. It is therefore recommended to use the `SUSPENDED` macro which is exit-safe. It is used in the following way:
 
 ```{.cpp}
 	{  SUSPENDED
 
       // Modify the internal state,
       // return, or throw an exceptions as you like
    }
 ```

 ## Synchronization
 
 The Thread class is responsible for timing synchronization. I.e., it has to ensure that the proper amount of frames are executed per second. To synchronize timing, the thread waits for an external wake-up signal. When the wake-up signal comes in (by calling [wakeUp](@ref vc64::Thread::wakeUp), the thread computes the computes computes all missing frames. If the number of missing frames exceeds a threshold, the thread resynchronizes itself by calling [resync](@ref vc64::Thread::resync).
 
 ## Warp mode
 
 To speed up emulation (e.g., during disk accesses), the emulator may be put into warp mode. In this mode, timing synchronization is disabled causing the emulator to run as fast as possible.
 
 Similar to warp mode, the emulator may be put into track mode. This mode is enabled when the GUI debugger is opend and disabled when the debugger is closed. In track mode, several time-consuming tasks are performed that are usually left out. E.g., the CPU tracks all executed instructions and stores the recorded information in a trace buffer.
 
<div class="section_buttons">

| Previous                      |                                    Next |
|:------------------------------|----------------------------------------:|
| [Headless App](headless.md)   | [Communication Model](communication.md) |
 
</div>
