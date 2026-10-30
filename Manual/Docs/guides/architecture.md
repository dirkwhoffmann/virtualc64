# Architecture {#guide_architecture}

A typical app that uses the VirtualC64 core consists of two kinds of threads:

- The **user threads** are the threads of the front-end. They call the API functions. Any thread except the emulator thread is a user thread.
- The **emulator thread** is created by [launch()](@ref vc64::VirtualC64::launch). It lives as long as the emulator object and computes frames and audio samples.

The threads never touch each other's data directly. Requests that change the state of the emulator are handed over through a *command queue* and carried out by the emulator thread. The emulator reports events back through a *message callback*.

@image html architecture.svg ""

The diagram shows the three situations a front-end deals with:

1. **Launch.** Calling [launch()](@ref vc64::VirtualC64::launch) creates the emulator thread and registers the message callback. The callback is the only code of the front-end that runs on the emulator thread. It must not call back into the API, but forward the message to a user thread.
2. **Asynchronous call.** Functions with an `Async` suffix, such as [runAsync()](@ref vc64::VirtualC64::runAsync), put a command into the queue and return at once. The emulator thread picks it up at the start of its next update cycle, so a state change never happens in the middle of a frame. The outcome arrives later as a message.
3. **Blocking call.** Functions without a suffix, such as [powerOff()](@ref vc64::VirtualC64::powerOff), queue the same command, but wait until it has been carried out. They return the outcome directly, or throw an exception.

Reading the state of the emulator does not involve the queue. Getters such as [isRunning()](@ref vc64::VirtualC64::isRunning) read the data directly, on the calling thread.

The details are described in the following chapters:

- [Communication Model](communication.md) covers the command queue, the message queue, and direct access, including the rules for blocking and asynchronous functions and for reading the state.
- [State Model](state.md) describes the life cycle of the emulator thread, including suspension, synchronization, and warp mode.
