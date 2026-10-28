---
title: "VideoPortAPI"
---
# VideoPortAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class VideoPortAPI
```

Defined in VirtualC64.h:698

Video Port Public [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`lockTexture`](#locktexture)  | Locks the emulator texture. |
| `void` | [`unlockTexture`](#unlocktexture)  | Unlocks the emulator texture. |
| `const u32 *` | [`getTexture`](#gettexture) `const` | Returns a pointer to the most recent stable texture. |
| `const u32 *` | [`getTexture`](#gettexture-1) `const` | Returns the most recent stable texture together with its frame number. |
| `const u32 *` | [`getXrayTexture`](#getxraytexture) `const` | Returns a pointer to the most recent stable X-Ray texture. |
| `const u32 *` | [`getXrayTexture`](#getxraytexture-1) `const` | Returns the most recent stable X-Ray texture together with its frame number. |
| `void` | [`saveTexture`](#savetexture) `const` | Saves the most recent stable texture as a PNG file. |
| `void` | [`saveTexture`](#savetexture-1) `const` | Saves a cutout of the most recent stable texture as a PNG file (see above). |
| `void` | [`saveXrayTexture`](#savexraytexture) `const` | Saves the most recent stable X-Ray texture as a PNG file. |
| `void` | [`saveXrayTexture`](#savexraytexture-1) `const` | Saves a cutout of the most recent stable X-Ray texture as a PNG file. |
| `void` | [`findInnerArea`](#findinnerarea) `const` | Determines the area of the texture that contains picture data. |
| `void` | [`findInnerAreaNormalized`](#findinnerareanormalized) `const` | Same as [findInnerArea()](#findinnerarea), but returns normalized coordinates. |
| const [`VideoPortConfig`](vc64-VideoPortConfig.md#videoportconfig) & | [`getConfig`](#getconfig-8) `const` | Returns the component's current configuration. |
| const [`VideoPortInfo`](vc64-VideoPortInfo.md#videoportinfo) & | [`getInfo`](#getinfo-10) `const` | Returns the component's current state. |
| const [`VideoPortInfo`](vc64-VideoPortInfo.md#videoportinfo) & | [`getCachedInfo`](#getcachedinfo-10) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-10)). |

---

### lockTexture

```cpp
void lockTexture()
```

Defined in VirtualC64.h:731

Locks the emulator texture.

This function aquires a mutex that prevents the emulator to modify the stable texture. Call this function prior to [getTexture()](#gettexture).

---

### unlockTexture

```cpp
void unlockTexture()
```

Defined in VirtualC64.h:738

Unlocks the emulator texture.

This function releases the mutex acquired in [lockTexture()](#locktexture). Call this function when the pointer returned by [getTexture()](#gettexture) is no longer needed.

---

### getTexture

`const`

```cpp
const u32 * getTexture() const
```

Defined in VirtualC64.h:746

Returns a pointer to the most recent stable texture.

The texture dimensions are given by constants vc64::Tex::width and vc64::Tex::height texels. Each texel is represented by a 32 bit color value.

---

### getTexture

`const`

```cpp
const u32 * getTexture(isize * nr) const
```

Defined in VirtualC64.h:751

Returns the most recent stable texture together with its frame number.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize *` | Receives the frame number. |

---

### getXrayTexture

`const`

```cpp
const u32 * getXrayTexture() const
```

Defined in VirtualC64.h:757

Returns a pointer to the most recent stable X-Ray texture.

The texture has the same dimensions as the regular texture.

---

### getXrayTexture

`const`

```cpp
const u32 * getXrayTexture(isize * nr) const
```

Defined in VirtualC64.h:763

Returns the most recent stable X-Ray texture together with its frame number.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize *` | Receives the frame number, which matches the number of the regular texture of the same frame. |

---

### saveTexture

`const`

```cpp
void saveTexture(const std::filesystem::path & path) const
```

Defined in VirtualC64.h:772

Saves the most recent stable texture as a PNG file.

The cutout is given in texels of the texture. The upper bounds x2 and y2 are exclusive. Without a cutout, the entire texture is saved.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be written. |

---

### saveTexture

`const`

```cpp
void saveTexture(const std::filesystem::path & path, isize x1, isize x2, isize y1, isize y2) const
```

Defined in VirtualC64.h:774

Saves a cutout of the most recent stable texture as a PNG file (see above).

---

### saveXrayTexture

`const`

```cpp
void saveXrayTexture(const std::filesystem::path & path) const
```

Defined in VirtualC64.h:781

Saves the most recent stable X-Ray texture as a PNG file.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be written. |

---

### saveXrayTexture

`const`

```cpp
void saveXrayTexture(const std::filesystem::path & path, isize x1, isize x2, isize y1, isize y2) const
```

Defined in VirtualC64.h:783

Saves a cutout of the most recent stable X-Ray texture as a PNG file.

---

### findInnerArea

`const`

```cpp
void findInnerArea(isize & x1, isize & x2, isize & y1, isize & y2) const
```

Defined in VirtualC64.h:802

Determines the area of the texture that contains picture data.

The result is meant to be used to crop the border of the picture. The [VirtualC64](vc64-VirtualC64.md#virtualc64) core does not analyze the texture yet: the functions always return the complete texture. The emulator thread is suspended during the call.

[findInnerArea()](#findinnerarea) returns the coordinates in texture pixels, [findInnerAreaNormalized()](#findinnerareanormalized) returns them relative to the size of the texture (0.0 ... 1.0).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `x1` | `isize &` | Left edge of the picture area |
| `x2` | `isize &` | Right edge of the picture area |
| `y1` | `isize &` | Top edge of the picture area |
| `y2` | `isize &` | Bottom edge of the picture area |

---

### findInnerAreaNormalized

`const`

```cpp
void findInnerAreaNormalized(double & x1, double & x2, double & y1, double & y2) const
```

Defined in VirtualC64.h:804

Same as [findInnerArea()](#findinnerarea), but returns normalized coordinates.

---

### getConfig

`const`

```cpp
const VideoPortConfig & getConfig() const
```

Defined in VirtualC64.h:708

Returns the component's current configuration.

---

### getInfo

`const`

```cpp
const VideoPortInfo & getInfo() const
```

Defined in VirtualC64.h:719

Returns the component's current state.

[getInfo()](#getinfo-10) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-10) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const VideoPortInfo & getCachedInfo() const
```

Defined in VirtualC64.h:721

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-10)).

