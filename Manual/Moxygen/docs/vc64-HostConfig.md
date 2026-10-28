---
title: "HostConfig"
---
# HostConfig

```cpp
#include <Infrastructure/HostTypes.h>
```

```cpp
struct HostConfig
```

Defined in Infrastructure/HostTypes.h:69

Properties of the host machine.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`refreshRate`](#refreshrate)  | Refresh rate of the host display. |
| `isize` | [`sampleRate`](#samplerate)  | Audio sample rate of the host computer. |
| [`TexFormat`](grp_audiovideo.md#texformat-1) | [`texFormat`](#texformat)  | Texture format. |
| `isize` | [`frameBufferWidth`](#framebufferwidth)  | Width of the host frame buffer (in pixels). |
| `isize` | [`frameBufferHeight`](#framebufferheight)  | Height of the host frame buffer (in pixels). |

---

### refreshRate

```cpp
isize refreshRate
```

Defined in Infrastructure/HostTypes.h:72

Refresh rate of the host display.

---

### sampleRate

```cpp
isize sampleRate
```

Defined in Infrastructure/HostTypes.h:75

Audio sample rate of the host computer.

---

### texFormat

```cpp
TexFormat texFormat
```

Type: [`TexFormat`](grp_audiovideo.md#texformat-1)

Defined in Infrastructure/HostTypes.h:78

Texture format.

---

### frameBufferWidth

```cpp
isize frameBufferWidth
```

Defined in Infrastructure/HostTypes.h:81

Width of the host frame buffer (in pixels).

---

### frameBufferHeight

```cpp
isize frameBufferHeight
```

Defined in Infrastructure/HostTypes.h:82

Height of the host frame buffer (in pixels).

