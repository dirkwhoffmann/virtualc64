---
title: "PaddleAPI"
---
# PaddleAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class PaddleAPI
```

Defined in VirtualC64.h:1032

Paddle Public [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`PaddleInfo`](vc64-PaddleInfo.md#paddleinfo) & | [`getInfo`](#getinfo-13) `const` | Returns the component's current state. |
| const [`PaddleInfo`](vc64-PaddleInfo.md#paddleinfo) & | [`getCachedInfo`](#getcachedinfo-13) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-13)). |

---

### getInfo

`const`

```cpp
const PaddleInfo & getInfo() const
```

Defined in VirtualC64.h:1049

Returns the component's current state.

[getInfo()](#getinfo-13) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-13) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const PaddleInfo & getCachedInfo() const
```

Defined in VirtualC64.h:1051

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-13)).

