---
title: "VICIIAPI"
kind: class
---
# VICIIAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class VICIIAPI
```

Defined in VirtualC64.h:500

VICII [API](/reference/vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`VICIITraits`](/reference/vc64-VICIITraits.md#viciitraits) & | [`getTraits`](#gettraits) `const` | Provides details about the currently selected VICII revision. |
| const [`VICIIConfig`](/reference/vc64-VICIIConfig.md#viciiconfig) & | [`getConfig`](#getconfig-8) `const` | Returns the component's current configuration. |
| const [`VICIIInfo`](/reference/vc64-VICIIInfo.md#viciiinfo) & | [`getInfo`](#getinfo-13) `const` | Returns the component's current state. |
| const [`VICIIInfo`](/reference/vc64-VICIIInfo.md#viciiinfo) & | [`getCachedInfo`](#getcachedinfo-13) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-13)). |
| [`SpriteInfo`](/reference/vc64-SpriteInfo.md#spriteinfo) | [`getSpriteInfo`](#getspriteinfo) `const` | Returns information about a sprite. |
| `u32` | [`getColor`](#getcolor) `const` | Returns the RGBA value of a color. |
| `u32` | [`getColor`](#getcolor-1) `const` | Returns the RGBA value of a color from a color palette. |

---

### getTraits

`const`

```cpp
const VICIITraits & getTraits() const
```

Defined in VirtualC64.h:510

Provides details about the currently selected VICII revision.

---

### getConfig

`const`

```cpp
const VICIIConfig & getConfig() const
```

Defined in VirtualC64.h:514

Returns the component's current configuration.

---

### getInfo

`const`

```cpp
const VICIIInfo & getInfo() const
```

Defined in VirtualC64.h:525

Returns the component's current state.

[getInfo()](#getinfo-13) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-13) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const VICIIInfo & getCachedInfo() const
```

Defined in VirtualC64.h:527

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-13)).

---

### getSpriteInfo

`const`

```cpp
SpriteInfo getSpriteInfo(isize nr) const
```

Defined in VirtualC64.h:532

Returns information about a sprite.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | Number of the sprite (0 .. 7) |

---

### getColor

`const`

```cpp
u32 getColor(isize nr) const
```

Defined in VirtualC64.h:536

Returns the RGBA value of a color.

---

### getColor

`const`

```cpp
u32 getColor(isize nr, Palette palette) const
```

Defined in VirtualC64.h:540

Returns the RGBA value of a color from a color palette.

