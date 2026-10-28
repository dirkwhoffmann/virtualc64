---
title: "VICIITraits"
kind: struct
---
# VICIITraits

```cpp
#include <Components/VICII/VICIITypes.h>
```

```cpp
struct VICIITraits
```

Defined in Components/VICII/VICIITypes.h:314

Properties of a VICII chip revision.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`pal`](#pal)  | The chip is a [PAL](vc64-PAL.md#pal-1) chip. |
| `isize` | [`frequency`](#frequency)  | Clock frequency (in Hz). |
| `double` | [`fps`](#fps-1)  | Frames per second. |
| `isize` | [`linesPerFrame`](#linesperframe)  | Number of raster lines per frame. |
| `isize` | [`cyclesPerLine`](#cyclesperline)  | Number of cycles per raster line. |
| `isize` | [`cyclesPerFrame`](#cyclesperframe)  | Number of cycles per frame. |
| `isize` | [`visibleLines`](#visiblelines)  | Number of visible raster lines. |
| `bool` | [`grayCodeBug`](#graycodebug)  | The chip has the gray-code bug. |
| `bool` | [`delayedLpIrqs`](#delayedlpirqs)  | Light pen interrupts are delayed. |

---

### pal

```cpp
bool pal
```

Defined in Components/VICII/VICIITypes.h:317

The chip is a [PAL](vc64-PAL.md#pal-1) chip.

---

### frequency

```cpp
isize frequency
```

Defined in Components/VICII/VICIITypes.h:318

Clock frequency (in Hz).

---

### fps

```cpp
double fps
```

Defined in Components/VICII/VICIITypes.h:319

Frames per second.

---

### linesPerFrame

```cpp
isize linesPerFrame
```

Defined in Components/VICII/VICIITypes.h:322

Number of raster lines per frame.

---

### cyclesPerLine

```cpp
isize cyclesPerLine
```

Defined in Components/VICII/VICIITypes.h:323

Number of cycles per raster line.

---

### cyclesPerFrame

```cpp
isize cyclesPerFrame
```

Defined in Components/VICII/VICIITypes.h:324

Number of cycles per frame.

---

### visibleLines

```cpp
isize visibleLines
```

Defined in Components/VICII/VICIITypes.h:325

Number of visible raster lines.

---

### grayCodeBug

```cpp
bool grayCodeBug
```

Defined in Components/VICII/VICIITypes.h:328

The chip has the gray-code bug.

---

### delayedLpIrqs

```cpp
bool delayedLpIrqs
```

Defined in Components/VICII/VICIITypes.h:329

Light pen interrupts are delayed.

