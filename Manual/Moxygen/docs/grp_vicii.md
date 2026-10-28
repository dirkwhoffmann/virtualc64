---
title: "VICII"
---
# VICII

The VICII graphics chip and the X-Ray visualizer.

## Classes

| Name | Description |
|------|-------------|
| [`VICIIAPI`](vc64-VICIIAPI.md#viciiapi) | VICII [API](vc64-API.md#api) |
| [`XRayAPI`](vc64-XRayAPI.md#xrayapi) | DMA Debugger Public [API](vc64-API.md#api) |
| [`FrameFlipflops`](vc64-FrameFlipflops.md#frameflipflops) | State of the frame flip-flops. |
| [`SpriteInfo`](vc64-SpriteInfo.md#spriteinfo) | State of a sprite. |
| [`VICIIConfig`](vc64-VICIIConfig.md#viciiconfig) | Configuration of the VICII chip. |
| [`VICIIInfo`](vc64-VICIIInfo.md#viciiinfo) | State of the VICII chip. |
| [`VICIIMetrics`](vc64-VICIIMetrics.md#viciimetrics) | Performance statistics of the graphics synthesizer. |
| [`VICIITraits`](vc64-VICIITraits.md#viciitraits) | Properties of a VICII chip revision. |
| [`XRayConfig`](vc64-XRayConfig.md#xrayconfig) | Configuration of the X-Ray visualizer. |

## Enumerations

| Name | Description |
|------|-------------|
| [`ColorSource`](#colorsource)  | Source of a pixel color in the graphics synthesizer. |
| [`DisplayMode`](#displaymode)  | C64 display mode. |
| [`DmaDisplayMode`](#dmadisplaymode)  | DMA debugger display mode. |
| [`GlueLogic`](#gluelogic)  | Glue logic implementation. |
| [`ScreenGeometry`](#screengeometry)  | C64 canvas size. |
| [`VICIIColor`](#viciicolor)  | The 16 colors of the VICII chip. |
| [`VICIIRev`](#viciirev)  | VICII chip revision. |
| [`XRayCompositor`](#xraycompositor)  | Who merges the X-Ray texture into the emulator texture. |
| [`XRayMode`](#xraymode)  | X-Ray mode. |

---

### ColorSource

```cpp
enum ColorSource
```

Defined in Components/VICII/VICIITypes.h:226

Source of a pixel color in the graphics synthesizer.

| Value | Description |
|-------|-------------|
| `D021` | Color comes from background color register. |
| `D022` | Color comes from first extended color register. |
| `D023` | Color comes from second extended color register. |
| `CHAR_LO` | Color comes from the low byte of the fetched character. |
| `CHAR_HI` | Color comes from the high byte of the fetched character. |
| `COLRAM3` | Color comes from the color RAM (lower three 3 bits). |
| `COLRAM4` | Color comes from the color RAM (all 4 bits). |
| `INDEXED` | Color comes from a color register. |
| `ZERO` | Invalid display modes. |

---

### DisplayMode

```cpp
enum DisplayMode
```

Defined in Components/VICII/VICIITypes.h:186

C64 display mode.

| Value | Description |
|-------|-------------|
| `STANDARD_TEXT` | Standard Character Mode. |
| `MULTICOLOR_TEXT` | Multicolor Character Mode. |
| `STANDARD_BITMAP` | Standard Bitmap Mode. |
| `MULTICOLOR_BITMAP` | Multicolor Bitmap Mode. |
| `EXTENDED_BG_COLOR` | Extended Background Color Mode. |
| `INVALID_TEXT` | Extended Background Color Multicolor Character Mode. |
| `INV_STANDARD_BITMAP` | Extended Background Color Standard Bitmap Mode. |
| `INV_MULTICOL_BITMAP` | Extended Background Color Multicolor Bitmap Mode. |

---

### DmaDisplayMode

```cpp
enum DmaDisplayMode
```

Defined in Components/VICII/XRayTypes.h:96

DMA debugger display mode.

| Value | Description |
|-------|-------------|
| `FG_LAYER` |  |
| `BG_LAYER` |  |
| `ODD_EVEN_LAYERS` |  |

---

### GlueLogic

```cpp
enum GlueLogic
```

Defined in Components/VICII/VICIITypes.h:126

Glue logic implementation.

| Value | Description |
|-------|-------------|
| `DISCRETE` | Discrete logic. |
| `IC` | Integrated Circuit (PLA). |

---

### ScreenGeometry

```cpp
enum ScreenGeometry
```

Defined in Components/VICII/VICIITypes.h:154

C64 canvas size.

| Value | Description |
|-------|-------------|
| `GEOMETRY_25_40` | 25 rows x 40 columns |
| `GEOMETRY_25_38` | 25 rows x 38 columns |
| `GEOMETRY_24_40` | 24 rows x 40 columns |
| `GEOMETRY_24_38` | 24 rows x 38 columns |

---

### VICIIColor

```cpp
enum VICIIColor
```

Defined in Components/VICII/VICIITypes.h:268

The 16 colors of the VICII chip.

| Value | Description |
|-------|-------------|
| `BLACK` |  |
| `WHITE` |  |
| `RED` |  |
| `CYAN` |  |
| `PURPLE` |  |
| `GREEN` |  |
| `BLUE` |  |
| `YELLOW` |  |
| `ORANGE` |  |
| `BROWN` |  |
| `LIGHT_RED` |  |
| `DARK_GREY` |  |
| `GREY` |  |
| `LIGHT_GREEN` |  |
| `LIGHT_BLUE` |  |
| `LIGHT_GREY` |  |

---

### VICIIRev

```cpp
enum VICIIRev
```

Defined in Components/VICII/VICIITypes.h:90

VICII chip revision.

| Value | Description |
|-------|-------------|
| `PAL_6569_R1` | Early C64 [PAL](vc64-PAL.md#pal-1) models. |
| `PAL_6569_R3` | Later C64 [PAL](vc64-PAL.md#pal-1) models. |
| `PAL_8565` | C64 II [PAL](vc64-PAL.md#pal-1) models. |
| `NTSC_6567_R56A` | Early C64 [NTSC](vc64-NTSC.md#ntsc) models. |
| `NTSC_6567` | Later C64 [NTSC](vc64-NTSC.md#ntsc) models. |
| `NTSC_8562` | C64 II [NTSC](vc64-NTSC.md#ntsc) models. |

---

### XRayCompositor

```cpp
enum XRayCompositor
```

Defined in Components/VICII/XRayTypes.h:64

Who merges the X-Ray texture into the emulator texture.

| Value | Description |
|-------|-------------|
| `CPU` | The emulator does it. |
| `GPU` | The host does it (e.g., in its merge shader). |

---

### XRayMode

```cpp
enum XRayMode
```

Defined in Components/VICII/XRayTypes.h:29

X-Ray mode.

| Value | Description |
|-------|-------------|
| `XRAY_NONE` |  |
| `XRAY_DMA` |  |
| `XRAY_LAYERS` |  |
