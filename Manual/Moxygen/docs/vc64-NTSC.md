---
title: "NTSC"
---
# NTSC

[NTSC](#ntsc) related constants

## Variables

| Return | Name | Description |
|--------|------|-------------|
| `const long` | [`CLOCK_FREQUENCY`](#clock_frequency-1) `static` | Clock frequency in Hz. |
| `const long` | [`CYCLES_PER_SECOND`](#cycles_per_second-1) `static` | CPU cycles per seconds. |
| `const double` | [`ASPECT_RATIO`](#aspect_ratio-1) `static` | Pixel aspect ratio. |
| `const long` | [`HBLANK_CYCLES_LEFT`](#hblank_cycles_left-1) `static` | Left HBLANK width in cycles. |
| `const long` | [`BORDER_CYCLES_LEFT`](#border_cycles_left-1) `static` | Left border width in cycles. |
| `const long` | [`CANVAS_CYCLES`](#canvas_cycles-1) `static` | Main canvas width in cycles. |
| `const long` | [`BORDER_CYCLES_RIGHT`](#border_cycles_right-1) `static` | Right border width in cycles. |
| `const long` | [`HBLANK_CYCLES_RIGHT`](#hblank_cycles_right-1) `static` | Right HBLANK width in cycles. |
| `const long` | [`CYCLES_PER_LINE`](#cycles_per_line) `static` | 13 + 4 + 40 + 4 + 4 |
| `const long` | [`HBLANK_PIXELS_LEFT`](#hblank_pixels_left-1) `static` | Left HBLANK width in pixels. |
| `const long` | [`BORDER_PIXELS_LEFT`](#border_pixels_left-1) `static` | Left border width in pixels. |
| `const long` | [`CANVAS_PIXELS`](#canvas_pixels-1) `static` | Main canvas width in pixels. |
| `const long` | [`BORDER_PIXELS_RIGHT`](#border_pixels_right-1) `static` | Right border width in pixels. |
| `const long` | [`HBLANK_PIXELS_RIGHT`](#hblank_pixels_right-1) `static` | Right HBLANK width in pixels. |
| `const long` | [`FIRST_VISIBLE_PIXEL`](#first_visible_pixel-1) `static` | 104 |
| `const long` | [`VISIBLE_PIXELS`](#visible_pixels-1) `static` | 32 + 320 + 32 |
| `const long` | [`LAST_VISIBLE_PIXEL`](#last_visible_pixel-1) `static` | 104 + 32 + 320 + 31 |
| `const long` | [`PIXELS_PER_LINE`](#pixels_per_line-1) `static` | 104 + 32 + 320 + 32 + 32 |
| `const long` | [`FIRST_VISIBLE_LINE`](#first_visible_line-1) `static` | First line after VBLANK. |

---

### CLOCK_FREQUENCY

`static`

```cpp
const long CLOCK_FREQUENCY = 1022727
```

Defined in Infrastructure/Constants.h:64

Clock frequency in Hz.

---

### CYCLES_PER_SECOND

`static`

```cpp
const long CYCLES_PER_SECOND = CLOCK_FREQUENCY
```

Defined in Infrastructure/Constants.h:65

CPU cycles per seconds.

---

### ASPECT_RATIO

`static`

```cpp
const double ASPECT_RATIO = 0.7500
```

Defined in Infrastructure/Constants.h:68

Pixel aspect ratio.

---

### HBLANK_CYCLES_LEFT

`static`

```cpp
const long HBLANK_CYCLES_LEFT = 13
```

Defined in Infrastructure/Constants.h:71

Left HBLANK width in cycles.

---

### BORDER_CYCLES_LEFT

`static`

```cpp
const long BORDER_CYCLES_LEFT = 4
```

Defined in Infrastructure/Constants.h:72

Left border width in cycles.

---

### CANVAS_CYCLES

`static`

```cpp
const long CANVAS_CYCLES = 40
```

Defined in Infrastructure/Constants.h:73

Main canvas width in cycles.

---

### BORDER_CYCLES_RIGHT

`static`

```cpp
const long BORDER_CYCLES_RIGHT = 4
```

Defined in Infrastructure/Constants.h:74

Right border width in cycles.

---

### HBLANK_CYCLES_RIGHT

`static`

```cpp
const long HBLANK_CYCLES_RIGHT = 4
```

Defined in Infrastructure/Constants.h:75

Right HBLANK width in cycles.

---

### CYCLES_PER_LINE

`static`

```cpp
const long CYCLES_PER_LINE = 65
```

Defined in Infrastructure/Constants.h:78

13 + 4 + 40 + 4 + 4

---

### HBLANK_PIXELS_LEFT

`static`

```cpp
const long HBLANK_PIXELS_LEFT = 104
```

Defined in Infrastructure/Constants.h:81

Left HBLANK width in pixels.

---

### BORDER_PIXELS_LEFT

`static`

```cpp
const long BORDER_PIXELS_LEFT = 32
```

Defined in Infrastructure/Constants.h:82

Left border width in pixels.

---

### CANVAS_PIXELS

`static`

```cpp
const long CANVAS_PIXELS = 320
```

Defined in Infrastructure/Constants.h:83

Main canvas width in pixels.

---

### BORDER_PIXELS_RIGHT

`static`

```cpp
const long BORDER_PIXELS_RIGHT = 32
```

Defined in Infrastructure/Constants.h:84

Right border width in pixels.

---

### HBLANK_PIXELS_RIGHT

`static`

```cpp
const long HBLANK_PIXELS_RIGHT = 32
```

Defined in Infrastructure/Constants.h:85

Right HBLANK width in pixels.

---

### FIRST_VISIBLE_PIXEL

`static`

```cpp
const long FIRST_VISIBLE_PIXEL = 104
```

Defined in Infrastructure/Constants.h:88

104

---

### VISIBLE_PIXELS

`static`

```cpp
const long VISIBLE_PIXELS = 384
```

Defined in Infrastructure/Constants.h:89

32 + 320 + 32

---

### LAST_VISIBLE_PIXEL

`static`

```cpp
const long LAST_VISIBLE_PIXEL = 487
```

Defined in Infrastructure/Constants.h:90

104 + 32 + 320 + 31

---

### PIXELS_PER_LINE

`static`

```cpp
const long PIXELS_PER_LINE = 520
```

Defined in Infrastructure/Constants.h:91

104 + 32 + 320 + 32 + 32

---

### FIRST_VISIBLE_LINE

`static`

```cpp
const long FIRST_VISIBLE_LINE = 16
```

Defined in Infrastructure/Constants.h:94

First line after VBLANK.

