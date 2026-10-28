---
title: "PAL"
kind: namespace
---
# PAL

[PAL](#pal-1) related constants

## Variables

| Return | Name | Description |
|--------|------|-------------|
| `const long` | [`CLOCK_FREQUENCY`](#clock_frequency) `static` | Clock frequency in Hz. |
| `const long` | [`CYCLES_PER_SECOND`](#cycles_per_second) `static` | CPU cycles per seconds. |
| `const double` | [`ASPECT_RATIO`](#aspect_ratio) `static` | Pixel aspect ratio. |
| `const long` | [`HBLANK_CYCLES_LEFT`](#hblank_cycles_left) `static` | Left HBLANK width in cycles. |
| `const long` | [`BORDER_CYCLES_LEFT`](#border_cycles_left) `static` | Left border width in cycles. |
| `const long` | [`CANVAS_CYCLES`](#canvas_cycles) `static` | Main canvas width in cycles. |
| `const long` | [`BORDER_CYCLES_RIGHT`](#border_cycles_right) `static` | Right border width in cycles. |
| `const long` | [`HBLANK_CYCLES_RIGHT`](#hblank_cycles_right) `static` | Right HBLANK width in cycles. |
| `const long` | [`PAL_CYCLES_PER_LINE`](#pal_cycles_per_line) `static` | 13 + 4 + 40 + 4 + 2 |
| `const long` | [`HBLANK_PIXELS_LEFT`](#hblank_pixels_left) `static` | Left HBLANK width in pixels. |
| `const long` | [`BORDER_PIXELS_LEFT`](#border_pixels_left) `static` | Left border width in pixels. |
| `const long` | [`CANVAS_PIXELS`](#canvas_pixels) `static` | Main canvas width in pixels. |
| `const long` | [`BORDER_PIXELS_RIGHT`](#border_pixels_right) `static` | Right border width in pixels. |
| `const long` | [`HBLANK_PIXELS_RIGHT`](#hblank_pixels_right) `static` | Right HBLANK width in pixels. |
| `const long` | [`FIRST_VISIBLE_PIXEL`](#first_visible_pixel) `static` | 104 |
| `const long` | [`VISIBLE_PIXELS`](#visible_pixels) `static` | 32 + 320 + 32 |
| `const long` | [`LAST_VISIBLE_PIXEL`](#last_visible_pixel) `static` | 104 + 32 + 320 + 31 |
| `const long` | [`PIXELS_PER_LINE`](#pixels_per_line) `static` | 104 + 32 + 320 + 32 + 16 |
| `const long` | [`FIRST_VISIBLE_LINE`](#first_visible_line) `static` | First line after VBLANK. |

---

### CLOCK_FREQUENCY

`static`

```cpp
const long CLOCK_FREQUENCY = 985249
```

Defined in Infrastructure/Constants.h:25

Clock frequency in Hz.

---

### CYCLES_PER_SECOND

`static`

```cpp
const long CYCLES_PER_SECOND = CLOCK_FREQUENCY
```

Defined in Infrastructure/Constants.h:26

CPU cycles per seconds.

---

### ASPECT_RATIO

`static`

```cpp
const double ASPECT_RATIO = 0.9365
```

Defined in Infrastructure/Constants.h:29

Pixel aspect ratio.

---

### HBLANK_CYCLES_LEFT

`static`

```cpp
const long HBLANK_CYCLES_LEFT = 13
```

Defined in Infrastructure/Constants.h:32

Left HBLANK width in cycles.

---

### BORDER_CYCLES_LEFT

`static`

```cpp
const long BORDER_CYCLES_LEFT = 4
```

Defined in Infrastructure/Constants.h:33

Left border width in cycles.

---

### CANVAS_CYCLES

`static`

```cpp
const long CANVAS_CYCLES = 40
```

Defined in Infrastructure/Constants.h:34

Main canvas width in cycles.

---

### BORDER_CYCLES_RIGHT

`static`

```cpp
const long BORDER_CYCLES_RIGHT = 4
```

Defined in Infrastructure/Constants.h:35

Right border width in cycles.

---

### HBLANK_CYCLES_RIGHT

`static`

```cpp
const long HBLANK_CYCLES_RIGHT = 2
```

Defined in Infrastructure/Constants.h:36

Right HBLANK width in cycles.

---

### PAL_CYCLES_PER_LINE

`static`

```cpp
const long PAL_CYCLES_PER_LINE = 63
```

Defined in Infrastructure/Constants.h:39

13 + 4 + 40 + 4 + 2

---

### HBLANK_PIXELS_LEFT

`static`

```cpp
const long HBLANK_PIXELS_LEFT = 104
```

Defined in Infrastructure/Constants.h:42

Left HBLANK width in pixels.

---

### BORDER_PIXELS_LEFT

`static`

```cpp
const long BORDER_PIXELS_LEFT = 32
```

Defined in Infrastructure/Constants.h:43

Left border width in pixels.

---

### CANVAS_PIXELS

`static`

```cpp
const long CANVAS_PIXELS = 320
```

Defined in Infrastructure/Constants.h:44

Main canvas width in pixels.

---

### BORDER_PIXELS_RIGHT

`static`

```cpp
const long BORDER_PIXELS_RIGHT = 32
```

Defined in Infrastructure/Constants.h:45

Right border width in pixels.

---

### HBLANK_PIXELS_RIGHT

`static`

```cpp
const long HBLANK_PIXELS_RIGHT = 16
```

Defined in Infrastructure/Constants.h:46

Right HBLANK width in pixels.

---

### FIRST_VISIBLE_PIXEL

`static`

```cpp
const long FIRST_VISIBLE_PIXEL = 104
```

Defined in Infrastructure/Constants.h:49

104

---

### VISIBLE_PIXELS

`static`

```cpp
const long VISIBLE_PIXELS = 384
```

Defined in Infrastructure/Constants.h:50

32 + 320 + 32

---

### LAST_VISIBLE_PIXEL

`static`

```cpp
const long LAST_VISIBLE_PIXEL = 487
```

Defined in Infrastructure/Constants.h:51

104 + 32 + 320 + 31

---

### PIXELS_PER_LINE

`static`

```cpp
const long PIXELS_PER_LINE = 504
```

Defined in Infrastructure/Constants.h:52

104 + 32 + 320 + 32 + 16

---

### FIRST_VISIBLE_LINE

`static`

```cpp
const long FIRST_VISIBLE_LINE = 16
```

Defined in Infrastructure/Constants.h:55

First line after VBLANK.

