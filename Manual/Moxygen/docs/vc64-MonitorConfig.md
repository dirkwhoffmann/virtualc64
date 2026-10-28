---
title: "MonitorConfig"
---
# MonitorConfig

```cpp
#include <Peripherals/Monitor/MonitorTypes.h>
```

```cpp
struct MonitorConfig
```

Defined in Peripherals/Monitor/MonitorTypes.h:167

Configuration of the monitor emulation.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`Palette`](vc64.md#palette) | [`palette`](#palette-1)  | Color palette. |
| `isize` | [`brightness`](#brightness)  | Monitor brightness. |
| `isize` | [`contrast`](#contrast)  | Monitor contrast. |
| `isize` | [`saturation`](#saturation)  | Monitor saturation. |
| `isize` | [`hCenter`](#hcenter)  | Horizontal center. |
| `isize` | [`vCenter`](#vcenter)  | Vertical center. |
| `isize` | [`hZoom`](#hzoom)  | Horizontal zoom. |
| `isize` | [`vZoom`](#vzoom)  | Vertical zoom. |
| [`Upscaler`](vc64.md#upscaler) | [`upscaler`](#upscaler-1)  | Effects. |
| `bool` | [`blur`](#blur)  | Blur. |
| `isize` | [`blurRadius`](#blurradius)  | Blur radius. |
| `bool` | [`bloom`](#bloom)  | Bloom. |
| `isize` | [`bloomRadius`](#bloomradius)  | Bloom radius. |
| `isize` | [`bloomBrightness`](#bloombrightness)  | Bloom brightness. |
| `isize` | [`bloomWeight`](#bloomweight)  | Bloom weight. |
| [`Dotmask`](vc64.md#dotmask) | [`dotmask`](#dotmask-1)  | [Dotmask](vc64.md#dotmask). |
| `isize` | [`dotMaskBrightness`](#dotmaskbrightness)  | [Dotmask](vc64.md#dotmask) brightness. |
| [`Scanlines`](vc64.md#scanlines) | [`scanlines`](#scanlines-1)  | [Scanlines](vc64.md#scanlines). |
| `isize` | [`scanlineBrightness`](#scanlinebrightness)  | Scanline brightness. |
| `isize` | [`scanlineWeight`](#scanlineweight)  | Scanline weight. |
| `bool` | [`disalignment`](#disalignment)  | Cathode ray disalignment. |
| `isize` | [`disalignmentH`](#disalignmenth)  | Horizontal cathode ray shift. |
| `isize` | [`disalignmentV`](#disalignmentv)  | Vertical cathode ray shift. |

---

### palette

```cpp
Palette palette
```

Type: [`Palette`](vc64.md#palette)

Defined in Peripherals/Monitor/MonitorTypes.h:170

Color palette.

---

### brightness

```cpp
isize brightness
```

Defined in Peripherals/Monitor/MonitorTypes.h:171

Monitor brightness.

---

### contrast

```cpp
isize contrast
```

Defined in Peripherals/Monitor/MonitorTypes.h:172

Monitor contrast.

---

### saturation

```cpp
isize saturation
```

Defined in Peripherals/Monitor/MonitorTypes.h:173

Monitor saturation.

---

### hCenter

```cpp
isize hCenter
```

Defined in Peripherals/Monitor/MonitorTypes.h:176

Horizontal center.

---

### vCenter

```cpp
isize vCenter
```

Defined in Peripherals/Monitor/MonitorTypes.h:177

Vertical center.

---

### hZoom

```cpp
isize hZoom
```

Defined in Peripherals/Monitor/MonitorTypes.h:178

Horizontal zoom.

---

### vZoom

```cpp
isize vZoom
```

Defined in Peripherals/Monitor/MonitorTypes.h:179

Vertical zoom.

---

### upscaler

```cpp
Upscaler upscaler
```

Type: [`Upscaler`](vc64.md#upscaler)

Defined in Peripherals/Monitor/MonitorTypes.h:182

Effects.

---

### blur

```cpp
bool blur
```

Defined in Peripherals/Monitor/MonitorTypes.h:184

Blur.

---

### blurRadius

```cpp
isize blurRadius
```

Defined in Peripherals/Monitor/MonitorTypes.h:185

Blur radius.

---

### bloom

```cpp
bool bloom
```

Defined in Peripherals/Monitor/MonitorTypes.h:187

Bloom.

---

### bloomRadius

```cpp
isize bloomRadius
```

Defined in Peripherals/Monitor/MonitorTypes.h:188

Bloom radius.

---

### bloomBrightness

```cpp
isize bloomBrightness
```

Defined in Peripherals/Monitor/MonitorTypes.h:189

Bloom brightness.

---

### bloomWeight

```cpp
isize bloomWeight
```

Defined in Peripherals/Monitor/MonitorTypes.h:190

Bloom weight.

---

### dotmask

```cpp
Dotmask dotmask
```

Type: [`Dotmask`](vc64.md#dotmask)

Defined in Peripherals/Monitor/MonitorTypes.h:192

[Dotmask](vc64.md#dotmask).

---

### dotMaskBrightness

```cpp
isize dotMaskBrightness
```

Defined in Peripherals/Monitor/MonitorTypes.h:193

[Dotmask](vc64.md#dotmask) brightness.

---

### scanlines

```cpp
Scanlines scanlines
```

Type: [`Scanlines`](vc64.md#scanlines)

Defined in Peripherals/Monitor/MonitorTypes.h:195

[Scanlines](vc64.md#scanlines).

---

### scanlineBrightness

```cpp
isize scanlineBrightness
```

Defined in Peripherals/Monitor/MonitorTypes.h:196

Scanline brightness.

---

### scanlineWeight

```cpp
isize scanlineWeight
```

Defined in Peripherals/Monitor/MonitorTypes.h:197

Scanline weight.

---

### disalignment

```cpp
bool disalignment
```

Defined in Peripherals/Monitor/MonitorTypes.h:199

Cathode ray disalignment.

---

### disalignmentH

```cpp
isize disalignmentH
```

Defined in Peripherals/Monitor/MonitorTypes.h:200

Horizontal cathode ray shift.

---

### disalignmentV

```cpp
isize disalignmentV
```

Defined in Peripherals/Monitor/MonitorTypes.h:201

Vertical cathode ray shift.

