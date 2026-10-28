---
title: "Audio and video"
---
# Audio and video

The audio port, the video port, and the monitor emulation.

## Classes

| Name | Description |
|------|-------------|
| [`AudioPortAPI`](vc64-AudioPortAPI.md#audioportapi) | Audio Port Public [API](vc64-API.md#api) |
| [`VideoPortAPI`](vc64-VideoPortAPI.md#videoportapi) | Video Port Public [API](vc64-API.md#api) |
| [`AudioPortConfig`](vc64-AudioPortConfig.md#audioportconfig) | Configuration of the audio port. |
| [`AudioPortInfo`](vc64-AudioPortInfo.md#audioportinfo) | State of the audio port (currently carries no data). |
| [`AudioPortMetrics`](vc64-AudioPortMetrics.md#audioportmetrics) | Audio statistics. |
| [`MonitorConfig`](vc64-MonitorConfig.md#monitorconfig) | Configuration of the monitor emulation. |
| [`VideoPortConfig`](vc64-VideoPortConfig.md#videoportconfig) | Configuration of the video port. |
| [`VideoPortInfo`](vc64-VideoPortInfo.md#videoportinfo) | State of the video port. |
| [`VideoPortStats`](vc64-VideoPortStats.md#videoportstats) | Statistics of the video port. |

## Enumerations

| Name | Description |
|------|-------------|
| [`Dotmask`](#dotmask)  | [Dotmask](#dotmask). |
| [`Palette`](#palette)  | Color palette. |
| [`Scanlines`](#scanlines)  | [Scanlines](#scanlines). |
| [`TexFormat`](#texformat-1)  | Byte layout of a 32-bit host pixel. |
| [`Upscaler`](#upscaler)  | Pixel upscaler. |

---

### Dotmask

```cpp
enum Dotmask
```

Defined in Peripherals/Monitor/MonitorTypes.h:99

[Dotmask](#dotmask).

| Value | Description |
|-------|-------------|
| `NONE` | No dotmask. |
| `BISECTED` | [Dotmask](#dotmask) pattern 1. |
| `TRISECTED` | [Dotmask](#dotmask) pattern 2. |
| `BISECTED_SHIFTED` | [Dotmask](#dotmask) pattern 3. |
| `TRISECTED_SHIFTED` | [Dotmask](#dotmask) pattern 4. |

---

### Palette

```cpp
enum Palette
```

Defined in Peripherals/Monitor/MonitorTypes.h:25

Color palette.

| Value | Description |
|-------|-------------|
| `COLOR` | Standard C64 color palette. |
| `BLACK_WHITE` | Black and white monochrome palette. |
| `PAPER_WHITE` | Paper white monochrome palette. |
| `GREEN` | Green monochrome palette. |
| `AMBER` | Amber monochrome palette. |
| `SEPIA` | Sepia monochrome palette. |

---

### Scanlines

```cpp
enum Scanlines
```

Defined in Peripherals/Monitor/MonitorTypes.h:133

[Scanlines](#scanlines).

| Value | Description |
|-------|-------------|
| `NONE` | No scanlines. |
| `EMBEDDED` | Embed scanlines in the emulator texture. |
| `SUPERIMPOSE` | Emulate scanlines in the fragment shader. |

---

### TexFormat

```cpp
enum TexFormat
```

Defined in Infrastructure/HostTypes.h:29

Byte layout of a 32-bit host pixel.

| Value | Description |
|-------|-------------|
| `ABGR` | AABBGGRR. |
| `ARGB` | AARRGGBB. |
| `RGBA` | RRGGBBAA. |

---

### Upscaler

```cpp
enum Upscaler
```

Defined in Peripherals/Monitor/MonitorTypes.h:69

Pixel upscaler.

| Value | Description |
|-------|-------------|
| `NONE` | No upscaler. |
| `EPX_2X` | EPX upscaler (2x). |
| `XBR_4X` | XBR upscaler (4x). |
