---
title: "XRayConfig"
kind: struct
---
# XRayConfig

```cpp
#include <Components/VICII/XRayTypes.h>
```

```cpp
struct XRayConfig
```

Defined in Components/VICII/XRayTypes.h:136

Configuration of the X-Ray visualizer.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`XRayMode`](enum-XRayMode.md) | [`mode`](#mode)  | X-Ray mode (off, DMA debugger, layer inspector). |
| `bool` | [`dmaOverlay`](#dmaoverlay)  | Enable the DMA overlay. |
| [`XRayCompositor`](enum-XRayCompositor.md) | [`compositor`](#compositor)  | Compositor used to merge the overlay with the picture. |
| `bool` | [`dmaChannel`](#dmachannel)  | Visualize the DMA channel (one flag per channel) |
| `u32` | [`dmaColor`](#dmacolor)  | Colors of the DMA channels (0 - 5) and of the cut-out layers (0 - 10). The layers use sprite n : color n, border : 8, foreground : 9, background : 10. Hence, the colors of the first channels are shared. |
| [`DmaDisplayMode`](enum-DmaDisplayMode.md) | [`dmaDisplayMode`](#dmadisplaymode-1)  | DMA display mode. |
| `u8` | [`dmaOpacity`](#dmaopacity)  | Opacity of the DMA overlay. |
| `u16` | [`cutLayers`](#cutlayers)  | Layers cut out in XRayMode::XRAY_LAYERS Bits 0 - 7: sprites, 9: background, 10: foreground, 11: border |

---

### mode

```cpp
XRayMode mode
```

Type: [`XRayMode`](enum-XRayMode.md)

Defined in Components/VICII/XRayTypes.h:139

X-Ray mode (off, DMA debugger, layer inspector).

---

### dmaOverlay

```cpp
bool dmaOverlay
```

Defined in Components/VICII/XRayTypes.h:140

Enable the DMA overlay.

---

### compositor

```cpp
XRayCompositor compositor
```

Type: [`XRayCompositor`](enum-XRayCompositor.md)

Defined in Components/VICII/XRayTypes.h:141

Compositor used to merge the overlay with the picture.

---

### dmaChannel

```cpp
bool dmaChannel[6]
```

Defined in Components/VICII/XRayTypes.h:142

Visualize the DMA channel (one flag per channel)

---

### dmaColor

```cpp
u32 dmaColor[XRAY_COLOR_COUNT]
```

Defined in Components/VICII/XRayTypes.h:146

Colors of the DMA channels (0 - 5) and of the cut-out layers (0 - 10). The layers use sprite n : color n, border : 8, foreground : 9, background : 10. Hence, the colors of the first channels are shared.

---

### dmaDisplayMode

```cpp
DmaDisplayMode dmaDisplayMode
```

Type: [`DmaDisplayMode`](enum-DmaDisplayMode.md)

Defined in Components/VICII/XRayTypes.h:147

DMA display mode.

---

### dmaOpacity

```cpp
u8 dmaOpacity
```

Defined in Components/VICII/XRayTypes.h:148

Opacity of the DMA overlay.

---

### cutLayers

```cpp
u16 cutLayers
```

Defined in Components/VICII/XRayTypes.h:152

Layers cut out in XRayMode::XRAY_LAYERS Bits 0 - 7: sprites, 9: background, 10: foreground, 11: border

