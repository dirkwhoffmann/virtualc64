---
title: State Model
description: The execution states of the emulator thread
---


## Overview

After launching, the emulator runs as a single thread alongside the front-end. The thread lives as long as the emulator object, but it does not compute frames all the time. What it does is controlled by the execution state:

- **Uninit:** The emulator object exists, but the thread has not been launched yet (see [launch()](/api-reference/vc64-VirtualC64.html#launch) and [isLaunched()](/api-reference/vc64-VirtualC64.html#islaunched)).
- **Off:** The emulator is powered off.
- **Paused:** The emulator is powered on, but no frames are computed.
- **Running:** The emulator is powered on and computes frames.
- **Halted:** The emulator thread has been terminated. This is the last step of the shutdown procedure.

The states Off, Paused, Running, and Halted are the values of the enumeration [vc64::ExecState](/api-reference/grp_emulator.html#execstate). A state change is requested with the functions shown in the diagram. Like all state-changing functions, they come in a blocking and an asynchronous flavor (for example [run()](/api-reference/vc64-VirtualC64.html#run) and [runAsync()](/api-reference/vc64-VirtualC64.html#runasync), see [Threading and Errors](threading.md)).

![](images/state.svg)

The functions [isPoweredOff()](/api-reference/vc64-VirtualC64.html#ispoweredoff), [isPaused()](/api-reference/vc64-VirtualC64.html#ispaused), [isRunning()](/api-reference/vc64-VirtualC64.html#isrunning), and [isPoweredOn()](/api-reference/vc64-VirtualC64.html#ispoweredon) query the state. Running an emulator that is powered off switches it on first. If a required ROM is missing, [run()](/api-reference/vc64-VirtualC64.html#run) throws a [CoreError](/api-reference/vc64-CoreError.html). [checkReady()](/api-reference/vc64-VirtualC64.html#checkready) performs the same check without running the emulator.

## Suspension

Independent of the execution state, the emulator thread can be *suspended* for a short period of time with [suspend()](/api-reference/vc64-API.html#suspend). A suspended thread stops at a safe point (it is neither computing a frame nor carrying out commands) and continues once [resume()](/api-reference/vc64-API.html#resume) has been called. The execution state does not change. [isSuspended()](/api-reference/vc64-VirtualC64.html#issuspended) reports whether a suspension is active. See [Threading and Errors](threading.md) for the rules that apply to suspensions.

## Synchronization

The emulator thread does not run freely. To compute the proper number of frames per second, it waits for an external wake-up signal and then computes all frames that are missing. The front-end sends the signal by calling [wakeUp()](/api-reference/vc64-VirtualC64.html#wakeup). The best moment is right after the current texture has been handed over to the GPU, which usually coincides with a VSYNC pulse. If the thread falls too far behind, it resynchronizes itself instead of trying to catch up.

## Warp mode

To speed up emulation (e.g., during disk accesses), the emulator can be put into *warp mode*. In this mode, timing synchronization is disabled and the emulator runs as fast as possible. Several clients can request warp mode independently by using different sources (see [warpOn()](/api-reference/vc64-VirtualC64.html#warpon)). The mode stays on as long as at least one source has requested it. The option [C64_WARP_MODE](/api-reference/grp_emulator.html#opt) selects when warp mode is entered automatically.

## Track mode

Similar to warp mode, the emulator can be put into *track mode* (see [trackOn()](/api-reference/vc64-VirtualC64.html#trackon)). A GUI enables it while its debugger is open. In track mode, several time-consuming tasks are performed that are usually left out. For example, the CPU records all executed instructions in a trace buffer.
