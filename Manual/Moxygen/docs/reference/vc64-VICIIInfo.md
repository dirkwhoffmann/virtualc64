---
title: "VICIIInfo"
kind: struct
---
# VICIIInfo

```cpp
#include <Components/VICII/VICIITypes.h>
```

```cpp
struct VICIIInfo
```

Defined in Components/VICII/VICIITypes.h:374

State of the VICII chip.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `u16` | [`scanline`](#scanline)  | Current raster line. |
| `u8` | [`rasterCycle`](#rastercycle)  | Current raster cycle. |
| `u32` | [`yCounter`](#ycounter)  | Raster line counter. |
| `u16` | [`xCounter`](#xcounter)  | Horizontal position counter. |
| `u16` | [`vc`](#vc)  | Video counter (VC). |
| `u16` | [`vcBase`](#vcbase)  | Video counter base (VCBASE). |
| `u8` | [`rc`](#rc)  | Row counter (RC). |
| `u8` | [`vmli`](#vmli)  | Video matrix line index (VMLI). |
| `u8` | [`ctrl1`](#ctrl1)  | Control register 1. |
| `u8` | [`ctrl2`](#ctrl2)  | Control register 2. |
| `u8` | [`dy`](#dy)  | Vertical scroll value. |
| `u8` | [`dx`](#dx)  | Horizontal scroll value. |
| `bool` | [`denBit`](#denbit)  | Display enable bit. |
| `bool` | [`badLine`](#badline)  | The current line is a bad line. |
| `bool` | [`displayState`](#displaystate)  | Display state (display or idle). |
| `bool` | [`vblank`](#vblank)  | Vertical blanking is active. |
| [`ScreenGeometry`](/reference/enum-ScreenGeometry.md) | [`screenGeometry`](#screengeometry-1)  | Screen geometry. |
| [`FrameFlipflops`](/reference/vc64-FrameFlipflops.md#frameflipflops) | [`frameFF`](#frameff)  | Frame flip-flops. |
| [`DisplayMode`](/reference/enum-DisplayMode.md) | [`displayMode`](#displaymode-1)  | Display mode. |
| `u8` | [`borderColor`](#bordercolor)  | Border color. |
| `u8` | [`bgColor0`](#bgcolor0)  | Background color 0. |
| `u8` | [`bgColor1`](#bgcolor1)  | Background color 1. |
| `u8` | [`bgColor2`](#bgcolor2)  | Background color 2. |
| `u8` | [`bgColor3`](#bgcolor3)  | Background color 3. |
| `u8` | [`memSelect`](#memselect)  | Memory control register. |
| `bool` | [`ultimax`](#ultimax)  | Ultimax memory mode is active. |
| `u16` | [`memoryBankAddr`](#memorybankaddr)  | Start address of the VIC bank. |
| `u16` | [`screenMemoryAddr`](#screenmemoryaddr)  | Address of the screen memory. |
| `u16` | [`charMemoryAddr`](#charmemoryaddr)  | Address of the character memory. |
| `u16` | [`irqLine`](#irqline)  | Raster line that triggers a raster interrupt. |
| `u8` | [`irr`](#irr)  | Interrupt request register. |
| `u8` | [`imr`](#imr-1)  | Interrupt mask register. |
| `u8` | [`latchedLPX`](#latchedlpx)  | Latched light pen X coordinate. |
| `u8` | [`latchedLPY`](#latchedlpy)  | Latched light pen Y coordinate. |
| `bool` | [`lpLine`](#lpline)  | State of the light pen line. |
| `bool` | [`lpIrqHasOccurred`](#lpirqhasoccurred)  | A light pen interrupt has occurred in this frame. |

---

### scanline

```cpp
u16 scanline
```

Defined in Components/VICII/VICIITypes.h:377

Current raster line.

---

### rasterCycle

```cpp
u8 rasterCycle
```

Defined in Components/VICII/VICIITypes.h:378

Current raster cycle.

---

### yCounter

```cpp
u32 yCounter
```

Defined in Components/VICII/VICIITypes.h:379

Raster line counter.

---

### xCounter

```cpp
u16 xCounter
```

Defined in Components/VICII/VICIITypes.h:380

Horizontal position counter.

---

### vc

```cpp
u16 vc
```

Defined in Components/VICII/VICIITypes.h:381

Video counter (VC).

---

### vcBase

```cpp
u16 vcBase
```

Defined in Components/VICII/VICIITypes.h:382

Video counter base (VCBASE).

---

### rc

```cpp
u8 rc
```

Defined in Components/VICII/VICIITypes.h:383

Row counter (RC).

---

### vmli

```cpp
u8 vmli
```

Defined in Components/VICII/VICIITypes.h:384

Video matrix line index (VMLI).

---

### ctrl1

```cpp
u8 ctrl1
```

Defined in Components/VICII/VICIITypes.h:387

Control register 1.

---

### ctrl2

```cpp
u8 ctrl2
```

Defined in Components/VICII/VICIITypes.h:388

Control register 2.

---

### dy

```cpp
u8 dy
```

Defined in Components/VICII/VICIITypes.h:389

Vertical scroll value.

---

### dx

```cpp
u8 dx
```

Defined in Components/VICII/VICIITypes.h:390

Horizontal scroll value.

---

### denBit

```cpp
bool denBit
```

Defined in Components/VICII/VICIITypes.h:391

Display enable bit.

---

### badLine

```cpp
bool badLine
```

Defined in Components/VICII/VICIITypes.h:392

The current line is a bad line.

---

### displayState

```cpp
bool displayState
```

Defined in Components/VICII/VICIITypes.h:393

Display state (display or idle).

---

### vblank

```cpp
bool vblank
```

Defined in Components/VICII/VICIITypes.h:394

Vertical blanking is active.

---

### screenGeometry

```cpp
ScreenGeometry screenGeometry
```

Type: [`ScreenGeometry`](/reference/enum-ScreenGeometry.md)

Defined in Components/VICII/VICIITypes.h:395

Screen geometry.

---

### frameFF

```cpp
FrameFlipflops frameFF
```

Type: [`FrameFlipflops`](/reference/vc64-FrameFlipflops.md#frameflipflops)

Defined in Components/VICII/VICIITypes.h:396

Frame flip-flops.

---

### displayMode

```cpp
DisplayMode displayMode
```

Type: [`DisplayMode`](/reference/enum-DisplayMode.md)

Defined in Components/VICII/VICIITypes.h:397

Display mode.

---

### borderColor

```cpp
u8 borderColor
```

Defined in Components/VICII/VICIITypes.h:398

Border color.

---

### bgColor0

```cpp
u8 bgColor0
```

Defined in Components/VICII/VICIITypes.h:399

Background color 0.

---

### bgColor1

```cpp
u8 bgColor1
```

Defined in Components/VICII/VICIITypes.h:400

Background color 1.

---

### bgColor2

```cpp
u8 bgColor2
```

Defined in Components/VICII/VICIITypes.h:401

Background color 2.

---

### bgColor3

```cpp
u8 bgColor3
```

Defined in Components/VICII/VICIITypes.h:402

Background color 3.

---

### memSelect

```cpp
u8 memSelect
```

Defined in Components/VICII/VICIITypes.h:405

Memory control register.

---

### ultimax

```cpp
bool ultimax
```

Defined in Components/VICII/VICIITypes.h:406

Ultimax memory mode is active.

---

### memoryBankAddr

```cpp
u16 memoryBankAddr
```

Defined in Components/VICII/VICIITypes.h:407

Start address of the VIC bank.

---

### screenMemoryAddr

```cpp
u16 screenMemoryAddr
```

Defined in Components/VICII/VICIITypes.h:408

Address of the screen memory.

---

### charMemoryAddr

```cpp
u16 charMemoryAddr
```

Defined in Components/VICII/VICIITypes.h:409

Address of the character memory.

---

### irqLine

```cpp
u16 irqLine
```

Defined in Components/VICII/VICIITypes.h:412

Raster line that triggers a raster interrupt.

---

### irr

```cpp
u8 irr
```

Defined in Components/VICII/VICIITypes.h:413

Interrupt request register.

---

### imr

```cpp
u8 imr
```

Defined in Components/VICII/VICIITypes.h:414

Interrupt mask register.

---

### latchedLPX

```cpp
u8 latchedLPX
```

Defined in Components/VICII/VICIITypes.h:417

Latched light pen X coordinate.

---

### latchedLPY

```cpp
u8 latchedLPY
```

Defined in Components/VICII/VICIITypes.h:418

Latched light pen Y coordinate.

---

### lpLine

```cpp
bool lpLine
```

Defined in Components/VICII/VICIITypes.h:419

State of the light pen line.

---

### lpIrqHasOccurred

```cpp
bool lpIrqHasOccurred
```

Defined in Components/VICII/VICIITypes.h:420

A light pen interrupt has occurred in this frame.

