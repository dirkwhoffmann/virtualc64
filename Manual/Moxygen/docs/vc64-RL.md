---
title: "RL"
kind: namespace
---
# RL

## Variables

| Return | Name | Description |
|--------|------|-------------|
| `u32` | [`STOP`](#stop) `constexpr` | Stop the run loop. |
| `u32` | [`WARP_ON`](#warp_on) `constexpr` | Switch on warp mode. |
| `u32` | [`WARP_OFF`](#warp_off) `constexpr` | Switch off warp mode. |
| `u32` | [`BREAKPOINT`](#breakpoint) `constexpr` | A breakpoint was reached. |
| `u32` | [`WATCHPOINT`](#watchpoint) `constexpr` | A watchpoint was reached. |
| `u32` | [`CPU_JAM`](#cpu_jam) `constexpr` | The CPU jammed. |
| `u32` | [`SINGLE_STEP`](#single_step) `constexpr` | Execute a single instruction. |
| `u32` | [`SINGLE_CYCLE`](#single_cycle) `constexpr` | Execute a single cycle. |
| `u32` | [`FINISH_LINE`](#finish_line) `constexpr` | Run until the end of the raster line. |
| `u32` | [`FINISH_FRAME`](#finish_frame) `constexpr` | Run until the end of the frame. |

---

### STOP

`constexpr`

```cpp
u32 STOP = (1 << 0)
```

Defined in Components/C64Types.h:378

Stop the run loop.

---

### WARP_ON

`constexpr`

```cpp
u32 WARP_ON = (1 << 1)
```

Defined in Components/C64Types.h:380

Switch on warp mode.

---

### WARP_OFF

`constexpr`

```cpp
u32 WARP_OFF = (1 << 2)
```

Defined in Components/C64Types.h:382

Switch off warp mode.

---

### BREAKPOINT

`constexpr`

```cpp
u32 BREAKPOINT = (1 << 3)
```

Defined in Components/C64Types.h:384

A breakpoint was reached.

---

### WATCHPOINT

`constexpr`

```cpp
u32 WATCHPOINT = (1 << 4)
```

Defined in Components/C64Types.h:386

A watchpoint was reached.

---

### CPU_JAM

`constexpr`

```cpp
u32 CPU_JAM = (1 << 5)
```

Defined in Components/C64Types.h:388

The CPU jammed.

---

### SINGLE_STEP

`constexpr`

```cpp
u32 SINGLE_STEP = (1 << 6)
```

Defined in Components/C64Types.h:390

Execute a single instruction.

---

### SINGLE_CYCLE

`constexpr`

```cpp
u32 SINGLE_CYCLE = (1 << 7)
```

Defined in Components/C64Types.h:392

Execute a single cycle.

---

### FINISH_LINE

`constexpr`

```cpp
u32 FINISH_LINE = (1 << 8)
```

Defined in Components/C64Types.h:394

Run until the end of the raster line.

---

### FINISH_FRAME

`constexpr`

```cpp
u32 FINISH_FRAME = (1 << 9)
```

Defined in Components/C64Types.h:396

Run until the end of the frame.

