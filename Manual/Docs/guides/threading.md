# Threading Model {#guide_threading}

After [launch()](@ref vc64::VirtualC64::launch), the emulator runs in a thread of its own, the *emulator thread*. All API functions must be called from a *user thread*, i.e., from any thread except the emulator thread. The only code that runs on the emulator thread is the message callback registered in [launch()](@ref vc64::VirtualC64::launch). It must not call back into the API. Forward the message to a user thread instead.

## Blocking and asynchronous functions

Requests that change the state of the emulator are carried out on the emulator thread, in the order in which they arrive. The name of a function tells how it hands over the request:

| Flavor | Example | Behavior |
|:-------|:--------|:---------|
| plain name | [powerOn()](@ref vc64::VirtualC64::powerOn) | Waits until the request has been carried out. Failures are thrown as exceptions. |
| `Async` suffix | [powerOnAsync()](@ref vc64::VirtualC64::powerOnAsync) | Queues the request and returns at once. |
| input events | [keyboard.press()](@ref vc64::KeyboardAPI::press) | Keyboard, mouse, joystick and RetroShell input is queued without a suffix. |

Because the queue is processed in order, a blocking call also guarantees that all earlier requests have been carried out. [sync()](@ref vc64::VirtualC64::sync) waits for them without issuing another request.

A blocking function throws a [CoreError](@ref vc64::CoreError) with code [NOT_LAUNCHED](@ref vc64::CoreError::NOT_LAUNCHED) if the emulator thread cannot carry out the request, i.e., if it has not been launched, has been halted, or has been suspended. Asynchronous requests can be issued in all these situations. If the thread has not been launched yet, they are carried out right after the launch.

An asynchronous request cannot throw to the caller. Most functions report the outcome with the message that is sent on success anyway (e.g., `SNAPSHOT_SAVED`). If the operation failed, the message carries the exception as the third argument of the callback:

```cpp
void process(const void *listener, Message msg, std::exception_ptr error)
{
    if (error) { /* the operation that triggered 'msg' has failed */ }
}
```

Input events have no such message. If the result matters, use the blocking counterpart.

## Reading the emulator state

Functions that query a single value, such as [isRunning()](@ref vc64::VirtualC64::isRunning), return the current value without synchronizing with the emulator thread. A value read while the emulator runs may be outdated at once.

Functions that return the state of a component come in two flavors:

- `getInfo()` reads the live state. The result is only consistent while the emulator is paused or suspended.
- `getCachedInfo()` returns the most recent snapshot recorded by the emulator thread. Use it while the emulator runs. The recorded components are selected with [setAutoInspectionMask()](@ref vc64::C64API::setAutoInspectionMask). The same holds for `getMetrics()` and `getCachedMetrics()`.

Both return references that can be overwritten later, so copy the data if it has to be kept.

## Suspending the emulator

To inspect several components consistently, pause the emulator thread for a short time:

```cpp
c64.suspend();

// Read the state of the components

c64.resume();
```

Suspensions can be nested. Each [suspend()](@ref vc64::API::suspend) must be followed by exactly one [resume()](@ref vc64::API::resume), so the code in between must not be left by an exception. Only one thread can hold a suspension at a time; others wait until it is released. The holder must not call a blocking API function, because the emulator thread cannot carry out the request. Such a call throws a CoreError instead of deadlocking. Use the asynchronous flavors instead.
