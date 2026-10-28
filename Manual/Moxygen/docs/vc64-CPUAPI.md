---
title: "CPUAPI"
---
# CPUAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class CPUAPI
```

Defined in VirtualC64.h:422

CPU [API](vc64-API.md#api)

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`CPUDebuggerAPI`](vc64-CPUDebuggerAPI.md#cpudebuggerapi) | [`debugger`](#debugger)  | CPU debugger. |
| [`GuardsAPI`](vc64-GuardsAPI.md#guardsapi) | [`breakpoints`](#breakpoints)  | Breakpoints. |
| [`GuardsAPI`](vc64-GuardsAPI.md#guardsapi) | [`watchpoints`](#watchpoints)  | Watchpoints. |

---

### debugger

```cpp
CPUDebuggerAPI debugger
```

Type: [`CPUDebuggerAPI`](vc64-CPUDebuggerAPI.md#cpudebuggerapi)

Defined in VirtualC64.h:431

CPU debugger.

---

### breakpoints

```cpp
GuardsAPI breakpoints
```

Type: [`GuardsAPI`](vc64-GuardsAPI.md#guardsapi)

Defined in VirtualC64.h:433

Breakpoints.

---

### watchpoints

```cpp
GuardsAPI watchpoints
```

Type: [`GuardsAPI`](vc64-GuardsAPI.md#guardsapi)

Defined in VirtualC64.h:435

Watchpoints.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`CPUConfig`](vc64-CPUConfig.md#cpuconfig) & | [`getConfig`](#getconfig-3) `const` | Returns the component's current configuration. |
| const [`CPUInfo`](vc64-CPUInfo.md#cpuinfo) & | [`getInfo`](#getinfo-3) `const` | Returns the component's current state. |
| const [`CPUInfo`](vc64-CPUInfo.md#cpuinfo) & | [`getCachedInfo`](#getcachedinfo-3) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-3)). |

---

### getConfig

`const`

```cpp
const CPUConfig & getConfig() const
```

Defined in VirtualC64.h:439

Returns the component's current configuration.

---

### getInfo

`const`

```cpp
const CPUInfo & getInfo() const
```

Defined in VirtualC64.h:450

Returns the component's current state.

[getInfo()](#getinfo-3) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-3) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const CPUInfo & getCachedInfo() const
```

Defined in VirtualC64.h:452

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-3)).

