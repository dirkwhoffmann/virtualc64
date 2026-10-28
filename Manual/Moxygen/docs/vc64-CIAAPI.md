---
title: "CIAAPI"
kind: class
---
# CIAAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class CIAAPI
```

Defined in VirtualC64.h:458

CIA [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`CIAConfig`](vc64-CIAConfig.md#ciaconfig) & | [`getConfig`](#getconfig-2) `const` | Returns the component's current configuration. |
| const [`CIAInfo`](vc64-CIAInfo.md#ciainfo) & | [`getInfo`](#getinfo-2) `const` | Returns the component's current state. |
| const [`CIAInfo`](vc64-CIAInfo.md#ciainfo) & | [`getCachedInfo`](#getcachedinfo-2) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-2)). |
| const [`CIAMetrics`](vc64-CIAMetrics.md#ciametrics) & | [`getMetrics`](#getmetrics-1) `const` | Returns statistical information about the components. |
| const [`CIAMetrics`](vc64-CIAMetrics.md#ciametrics) & | [`getCachedMetrics`](#getcachedmetrics-1) `const` | Returns the most recent metrics snapshot recorded by the emulator thread (see [getMetrics()](#getmetrics-1)). |

---

### getConfig

`const`

```cpp
const CIAConfig & getConfig() const
```

Defined in VirtualC64.h:468

Returns the component's current configuration.

---

### getInfo

`const`

```cpp
const CIAInfo & getInfo() const
```

Defined in VirtualC64.h:479

Returns the component's current state.

[getInfo()](#getinfo-2) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-2) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const CIAInfo & getCachedInfo() const
```

Defined in VirtualC64.h:481

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-2)).

---

### getMetrics

`const`

```cpp
const CIAMetrics & getMetrics() const
```

Defined in VirtualC64.h:492

Returns statistical information about the components.

[getMetrics()](#getmetrics-1) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedMetrics()](#getcachedmetrics-1) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedMetrics

`const`

```cpp
const CIAMetrics & getCachedMetrics() const
```

Defined in VirtualC64.h:494

Returns the most recent metrics snapshot recorded by the emulator thread (see [getMetrics()](#getmetrics-1)).

