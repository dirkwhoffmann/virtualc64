---
title: "C64Info"
kind: struct
---
# C64Info

```cpp
#include <Components/C64Types.h>
```

```cpp
struct C64Info
```

Defined in Components/C64Types.h:358

State of the C64 component.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`Cycle`](/reference/vc64.md#cycle) | [`cpuProgress`](#cpuprogress)  | CPU clock (in cycles). |
| [`Cycle`](/reference/vc64.md#cycle) | [`cia1Progress`](#cia1progress)  | Clock of CIA 1 (in cycles). |
| [`Cycle`](/reference/vc64.md#cycle) | [`cia2Progress`](#cia2progress)  | Clock of CIA 2 (in cycles). |
| `i64` | [`frame`](#frame)  | Frame number. |
| `long` | [`vpos`](#vpos)  | Vertical beam position (raster line). |
| `long` | [`hpos`](#hpos)  | Horizontal beam position (raster cycle). |
| [`EventSlotInfo`](/reference/vc64-EventSlotInfo.md#eventslotinfo) | [`slotInfo`](#slotinfo)  | Events. |

---

### cpuProgress

```cpp
Cycle cpuProgress
```

Type: [`Cycle`](/reference/vc64.md#cycle)

Defined in Components/C64Types.h:360

CPU clock (in cycles).

---

### cia1Progress

```cpp
Cycle cia1Progress
```

Type: [`Cycle`](/reference/vc64.md#cycle)

Defined in Components/C64Types.h:361

Clock of CIA 1 (in cycles).

---

### cia2Progress

```cpp
Cycle cia2Progress
```

Type: [`Cycle`](/reference/vc64.md#cycle)

Defined in Components/C64Types.h:362

Clock of CIA 2 (in cycles).

---

### frame

```cpp
i64 frame
```

Defined in Components/C64Types.h:363

Frame number.

---

### vpos

```cpp
long vpos
```

Defined in Components/C64Types.h:364

Vertical beam position (raster line).

---

### hpos

```cpp
long hpos
```

Defined in Components/C64Types.h:365

Horizontal beam position (raster cycle).

---

### slotInfo

```cpp
EventSlotInfo slotInfo[SLOT_COUNT]
```

Defined in Components/C64Types.h:368

Events.

