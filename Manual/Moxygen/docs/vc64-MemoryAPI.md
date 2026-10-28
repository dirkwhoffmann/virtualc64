---
title: "MemoryAPI"
---
# MemoryAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class MemoryAPI
```

Defined in VirtualC64.h:174

Memory [API](vc64-API.md#api)

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`MemoryDebuggerAPI`](vc64-MemoryDebuggerAPI.md#memorydebuggerapi) | [`debugger`](#debugger-1)  | Memory debugger. |

---

### debugger

```cpp
MemoryDebuggerAPI debugger
```

Type: [`MemoryDebuggerAPI`](vc64-MemoryDebuggerAPI.md#memorydebuggerapi)

Defined in VirtualC64.h:184

Memory debugger.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`MemConfig`](vc64-MemConfig.md#memconfig) & | [`getConfig`](#getconfig-4) `const` | Returns the component's current configuration. |
| const [`MemInfo`](vc64-MemInfo.md#meminfo) & | [`getInfo`](#getinfo-4) `const` | Returns the component's current state. |
| const [`MemInfo`](vc64-MemInfo.md#meminfo) & | [`getCachedInfo`](#getcachedinfo-4) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-4)). |
| `void` | [`drawHeatmap`](#drawheatmap) `const` | Draws a heatmap of the recent memory accesses. |

---

### getConfig

`const`

```cpp
const MemConfig & getConfig() const
```

Defined in VirtualC64.h:188

Returns the component's current configuration.

---

### getInfo

`const`

```cpp
const MemInfo & getInfo() const
```

Defined in VirtualC64.h:199

Returns the component's current state.

[getInfo()](#getinfo-4) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-4) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const MemInfo & getCachedInfo() const
```

Defined in VirtualC64.h:201

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-4)).

---

### drawHeatmap

`const`

```cpp
void drawHeatmap(u32 * buffer, isize width, isize height) const
```

Defined in VirtualC64.h:219

Draws a heatmap of the recent memory accesses.

The 64 KB address space is shown as a grid of 256 x 256 cells. Neighboring addresses are mapped to neighboring cells by interleaving the address bits, so that address ranges form compact blocks. A cell lights up when its address is accessed and fades out over time. The map is updated at the end of each frame, and only while the option [Opt::MEM_HEATMAP](grp_emulator.md#opt) is enabled.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `u32 *` | Texture buffer for 256 x 256 pixels (RGBA) |
| `width` | `isize` | Width of the texture, must be 256 |
| `height` | `isize` | Height of the texture, must be 256 |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::OPT_INV_ARG](vc64-CoreError.md#opt_inv_arg)) if the size is not 256 x 256. |

