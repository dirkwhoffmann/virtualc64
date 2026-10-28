---
title: "AudioPortAPI"
kind: class
---
# AudioPortAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class AudioPortAPI
```

Defined in VirtualC64.h:599

Audio Port Public [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`copyMono`](#copymono)  | Extracts a number of mono samples from the audio buffer Internally, the audio port maintains a ringbuffer storing stereo audio samples. When this function is used, both internal stream are added together and written to to the destination buffer. |
| `isize` | [`copyStereo`](#copystereo)  | Extracts a number of stereo samples from the audio buffer. |
| `isize` | [`copyInterleaved`](#copyinterleaved)  | Extracts a number of stereo samples from the audio buffer. This function has to be used if a stereo stream is managed in a single destination buffer. The samples of both channels will be interleaved, that is, a sample for the left channel will be followed by a sample of the right channel and vice versa. |
| `float` | [`drawL`](#drawl) `const` | Draws the waveform of the left or the right audio channel. |
| `float` | [`drawR`](#drawr) `const` | Draws the waveform of the right audio channel (see [drawL()](#drawl)). |
| const [`AudioPortConfig`](vc64-AudioPortConfig.md#audioportconfig) & | [`getConfig`](#getconfig) `const` | Returns the component's current configuration. |
| const [`AudioPortInfo`](vc64-AudioPortInfo.md#audioportinfo) & | [`getInfo`](#getinfo) `const` | Returns the component's current state. |
| const [`AudioPortInfo`](vc64-AudioPortInfo.md#audioportinfo) & | [`getCachedInfo`](#getcachedinfo) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo)). |
| const [`AudioPortMetrics`](vc64-AudioPortMetrics.md#audioportmetrics) & | [`getMetrics`](#getmetrics) `const` | Returns statistical information about the components. |
| const [`AudioPortMetrics`](vc64-AudioPortMetrics.md#audioportmetrics) & | [`getCachedMetrics`](#getcachedmetrics) `const` | Returns the most recent metrics snapshot recorded by the emulator thread (see [getMetrics()](#getmetrics)). |

---

### copyMono

```cpp
isize copyMono(float * buffer, isize n)
```

Defined in VirtualC64.h:649

Extracts a number of mono samples from the audio buffer Internally, the audio port maintains a ringbuffer storing stereo audio samples. When this function is used, both internal stream are added together and written to to the destination buffer.

#### Returns
Number of actually copied sound sound samples.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `float *` | Pointer to the destination buffer |
| `n` | `isize` | Number of sound samples to copy. |

---

### copyStereo

```cpp
isize copyStereo(float * left, float * right, isize n)
```

Defined in VirtualC64.h:657

Extracts a number of stereo samples from the audio buffer.

#### Returns
Number of actually copied sound sound samples.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `left` | `float *` | Pointer to the left channel's destination buffer. |
| `right` | `float *` | Pointer to the right channel's destination buffer. |
| `n` | `isize` | Number of sound samples to copy. |

---

### copyInterleaved

```cpp
isize copyInterleaved(float * buffer, isize n)
```

Defined in VirtualC64.h:668

Extracts a number of stereo samples from the audio buffer. This function has to be used if a stereo stream is managed in a single destination buffer. The samples of both channels will be interleaved, that is, a sample for the left channel will be followed by a sample of the right channel and vice versa.

#### Returns
Number of actually copied sound sound samples.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `float *` | Pointer to the destinationleft buffer. |
| `n` | `isize` | Number of sound samples to copy. |

---

### drawL

`const`

```cpp
float drawL(u32 * buffer, isize width, isize height, float maxAmp, u32 color) const
```

Defined in VirtualC64.h:688

Draws the waveform of the left or the right audio channel.

The waveform is plotted into a texture buffer. The functions return the highest amplitude found in the audio buffer. To implement auto-scaling, pass the returned value as parameter 'maxAmp' in the next call. The Mac app uses these functions to visualize the contents of the audio buffer in one of its inspector panels.

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

---

### drawR

`const`

```cpp
float drawR(u32 * buffer, isize width, isize height, float maxAmp, u32 color) const
```

Defined in VirtualC64.h:690

Draws the waveform of the right audio channel (see [drawL()](#drawl)).

---

### getConfig

`const`

```cpp
const AudioPortConfig & getConfig() const
```

Defined in VirtualC64.h:609

Returns the component's current configuration.

---

### getInfo

`const`

```cpp
const AudioPortInfo & getInfo() const
```

Defined in VirtualC64.h:620

Returns the component's current state.

[getInfo()](#getinfo) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const AudioPortInfo & getCachedInfo() const
```

Defined in VirtualC64.h:622

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo)).

---

### getMetrics

`const`

```cpp
const AudioPortMetrics & getMetrics() const
```

Defined in VirtualC64.h:633

Returns statistical information about the components.

[getMetrics()](#getmetrics) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedMetrics()](#getcachedmetrics) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedMetrics

`const`

```cpp
const AudioPortMetrics & getCachedMetrics() const
```

Defined in VirtualC64.h:635

Returns the most recent metrics snapshot recorded by the emulator thread (see [getMetrics()](#getmetrics)).

