# Communication Model {#guide_communication}


## Overview

The following diagram summarizes how the front-end and the emulator core exchange information.

@image html communication.svg ""

Three mechanisms are involved:

- **Command queue**

  Requests that change the state of the emulator travel through the command queue. The API function creates a command and appends it to the queue. The emulator thread carries out all pending commands at the beginning of each update cycle, one after the other, in the order in which they arrived. As a consequence, a state change never happens in the middle of a frame.

  The plain-name functions wait until their command has been carried out and pass on the outcome (see [Threading and Errors](threading.md)). The `Async` functions and the input events do not wait. The queue is bounded (65536 entries) to protect the emulator against a client that forgets to resume a suspended emulator thread. If the queue is full, new commands are dropped and the message `QUEUE_FULL` is sent once. Asynchronous requests may be queued before the emulator thread has been launched; they are carried out at launch, while the blocking flavors throw a CoreError.

- **Message queue**

  The emulator informs the front-end about important events by sending messages, for example when a snapshot has been taken, when a ROM has been loaded, or when RetroShell has finished a script. At launch, the front-end registers a callback function that receives all messages. The callback runs on the emulator thread and must not call into the API; it should forward the message to a user thread. Messages that report a failed operation carry the exception that caused it.

  Messages are not merged. The queue is bounded (65536 entries); a message that arrives at a full queue is dropped.

- **Direct access**

  Everything that is not a state change is read directly from the emulator, on the calling thread. This comprises queries such as [isRunning()](@ref vc64::VirtualC64::isRunning), the state of the components (`getInfo()` and `getCachedInfo()`), and large data such as textures and audio samples. The `getCachedInfo()` flavor returns a snapshot that the emulator thread records periodically and is the right choice while the emulator runs.

Which mechanism a function uses is part of its documentation. In general, plain-name and `Async` functions use the command queue, while getters use direct access. The sub-APIs ([vc64::CPUAPI](@ref vc64::CPUAPI), [vc64::VICIIAPI](@ref vc64::VICIIAPI), [vc64::SIDAPI](@ref vc64::SIDAPI), and so on) follow the same rules as the main class [vc64::VirtualC64](@ref vc64::VirtualC64).
