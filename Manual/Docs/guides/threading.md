# Threading and Errors {#guide_threading}


## Which thread calls what

After [launch()](@ref vc64::VirtualC64::launch), the emulator runs in a thread of its own, the *emulator thread*. All functions of the API must be called from a *user thread*, i.e., from any thread except the emulator thread. The only code that runs on the emulator thread is the message callback that is registered in [launch()](@ref vc64::VirtualC64::launch). It must not call back into the API. Forward the message to a user thread instead.

## Blocking and asynchronous functions

Requests that change the state of the emulator are always carried out on the emulator thread, in the order in which they arrive. The name of an API function tells how it hands over the request:

| Flavor | Example | Behavior |
|:-------|:--------|:---------|
| plain name | [powerOn()](@ref vc64::VirtualC64::powerOn) | Waits until the emulator thread has carried out the request. Failures are reported as exceptions. |
| `Async` suffix | [powerOnAsync()](@ref vc64::VirtualC64::powerOnAsync) | Queues the request and returns at once. A failure cannot be thrown to the caller. Most functions report the outcome with a message instead (see below). |
| input events | [keyboard.press()](@ref vc64::KeyboardAPI::press) | Keyboard, mouse, joystick and RetroShell input is queued without a suffix. |

Because the queue is processed in order, a blocking call also guarantees that all requests queued before it have been carried out. To wait for asynchronous requests without issuing another blocking call, use [sync()](@ref vc64::VirtualC64::sync).

A blocking function throws a [CoreError](@ref vc64::CoreError) with code [NOT_LAUNCHED](@ref vc64::CoreError::NOT_LAUNCHED) if the emulator thread cannot carry out the request. This is the case if the thread has not been launched, if it has been halted, if another thread has suspended it, or if the calling thread has suspended it itself. The asynchronous flavor can be used in all of these situations. If it is called before the emulator thread has been launched, the request is carried out right after the launch.

### Failures of asynchronous requests

An asynchronous request is carried out later, so an exception cannot be thrown to the caller. For most functions, the emulator reports the outcome with the message that is sent on success anyway, e.g., `SNAPSHOT_SAVED` after [saveSnapshotAsync()](@ref vc64::C64API::saveSnapshotAsync), or `ROM_LOADED` after [loadRomAsync()](@ref vc64::C64API::loadRomAsync). If the operation failed, the message carries the exception. The callback receives it as its third argument:

```cpp
void process(const void *listener, Message msg, std::exception_ptr error)
{
    if (error) { /* the operation that triggered 'msg' has failed */ }
}
```

Input events and a few simple requests have no such message. A failure of these is not reported. If the result matters, use the blocking counterpart.

## Reading the emulator state

Functions that only query a value, such as [isRunning()](@ref vc64::VirtualC64::isRunning) or [isWarping()](@ref vc64::VirtualC64::isWarping), return the current value without synchronizing with the emulator thread. A value that is read while the emulator runs may be outdated at once.

Functions that return the state of a component come in two flavors:

- `getInfo()` reads the live state. The result is only consistent while the emulator is paused or suspended.
- `getCachedInfo()` returns the most recent snapshot recorded by the emulator thread. This is the one to use while the emulator runs. The component classes that are recorded are selected with [setAutoInspectionMask()](@ref vc64::C64API::setAutoInspectionMask). The same holds for `getMetrics()` and `getCachedMetrics()`.

Both flavors return references that can be overwritten by a later call or by the emulator thread, so copy the data if it has to be kept.

Functions that read larger portions of the state or export it (for example `dump`, `disassemble`, or the `save...` functions) suspend the emulator thread for the duration of the call.

## Suspending the emulator

An application can pause the emulator thread for a short time, for example to inspect several components consistently:

```cpp
c64.suspend();

// Read the state of the components

c64.resume();
```

Suspensions can be nested. Each call to [suspend()](@ref vc64::API::suspend) must be followed by exactly one call to [resume()](@ref vc64::API::resume), so the code in between must not be left by an exception. Only one thread can hold a suspension at a time. Another thread that calls `suspend()`, or a blocking API function, waits until the holder has resumed the emulator.

The holder itself must not call a blocking API function while it holds the suspension, because the emulator thread cannot carry out the request. Such calls throw a CoreError instead of deadlocking. Use the asynchronous flavors or finish the suspension first.

## Error handling

API functions report failures by throwing exceptions that are derived from [utl::Error](@ref utl::Error):

| Exception | Meaning |
|:----------|:--------|
| [CoreError](@ref vc64::CoreError) | The request does not match the state of the emulator, e.g., an invalid option value, a missing ROM, or an emulator thread that cannot carry out the request. |
| [IOError](@ref utl::IOError) | A file or folder could not be accessed. |
| [MediaError](@ref vc64::MediaError) | A ROM, snapshot, disk image or similar file is invalid or not supported. |

Each function documents the exceptions it can throw with `@throw`. The error code of a CoreError is one of the constants declared in the class.

```cpp
try {
    c64.run();
} catch (const CoreError &e) {
    if (e.fault() == CoreError::ROM_KERNAL_MISSING) { /* ask the user for a ROM */ }
}
```

## Danger zone

The section *Danger zone* of [vc64::VirtualC64](@ref vc64::VirtualC64) contains functions that bypass the dedicated API: `put()` feeds raw commands into the command queue, and `call()` runs an arbitrary function on the emulator thread. They exist for situations that the API does not cover. What they touch is not covered by any compatibility promise, so prefer the dedicated functions.
