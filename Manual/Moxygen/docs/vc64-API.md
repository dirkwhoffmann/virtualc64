---
title: "API"
---
# API

```cpp
#include <VirtualC64.h>
```

```cpp
class API
```

Defined in VirtualC64.h:92

Base class of all APIs

Provides the suspend / resume mechanism that every [API](#api) class inherits.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`suspend`](#suspend) `const` | Suspends the emulator thread. |
| `void` | [`resume`](#resume) `const` | Resumes the emulator thread after a call to [suspend()](#suspend). |

---

### suspend

`const`

```cpp
void suspend() const
```

Defined in VirtualC64.h:121

Suspends the emulator thread.

Blocks until the emulator thread has stopped at a safe point, i.e., it is neither computing a frame nor processing its command queue. The call can be nested: the thread continues once [resume()](#resume) has been called as often as [suspend()](#suspend). Use it to read or modify the state of a component that is not protected by the [API](#api) itself.

Only one thread can hold a suspension at a time. Another thread that calls [suspend()](#suspend), or a blocking [API](#api) function that waits for the emulator thread, waits until the holder has resumed the emulator. The holder itself must not call such a function, because it would wait for itself: they throw a [CoreError](vc64-CoreError.md#coreerror), and so does halt(). Do not call [suspend()](#suspend) from the emulator thread.

---

### resume

`const`

```cpp
void resume() const
```

Defined in VirtualC64.h:125

Resumes the emulator thread after a call to [suspend()](#suspend).

