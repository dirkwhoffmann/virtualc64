---
title: "SIDAPI"
kind: class
---
# SIDAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class SIDAPI
```

Defined in VirtualC64.h:547

SID Public [API](/reference/vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `float` | [`draw`](#draw) `const` | Draws the waveform of a SID chip or of the audio output. |
| const [`SIDInfo`](/reference/vc64-SIDInfo.md#sidinfo) & | [`getInfo`](#getinfo-12) `const` | Returns the current state of a specific SID. |
| const [`SIDInfo`](/reference/vc64-SIDInfo.md#sidinfo) & | [`getCachedInfo`](#getcachedinfo-12) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-12)). |

---

### draw

`const`

```cpp
float draw(u32 * buffer, isize width, isize height, float maxAmp, u32 color, isize sid = -1) const
```

Defined in VirtualC64.h:590

Draws the waveform of a SID chip or of the audio output.

The waveform is plotted into a texture buffer. The function returns the highest amplitude found. To implement auto-scaling, pass the returned value as parameter 'maxAmp' in the next call.

#### Returns
The highest amplitude that was found.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `u32 *` | Texture buffer of size width * height |
| `width` | `isize` | Width of the texture in pixels |
| `height` | `isize` | Height of the texture in pixels |
| `maxAmp` | `float` | Amplitude that is mapped to the full height |
| `color` | `u32` | Color of the waveform |
| `sid` | `isize` | Number of the SID chip (0 - 3). Any other value draws the stereo audio output, with the left channel in the upper and the right channel in the lower half. |

---

### getInfo

`const`

```cpp
const SIDInfo & getInfo(isize nr) const
```

Defined in VirtualC64.h:567

Returns the current state of a specific SID.

[getInfo()](#getinfo-12) reads the live state on the calling thread and is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-12) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | SID number (0 - 3). 0 is the primary SID. |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::OPT_INV_ARG](/reference/vc64-CoreError.md#opt_inv_arg)) if the SID number is out of range. |

---

### getCachedInfo

`const`

```cpp
const SIDInfo & getCachedInfo(isize nr) const
```

Defined in VirtualC64.h:569

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-12)).

