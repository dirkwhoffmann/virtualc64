---
title: "EventSlotInfo"
---
# EventSlotInfo

```cpp
#include <Components/C64Types.h>
```

```cpp
struct EventSlotInfo
```

Defined in Components/C64Types.h:338

State of a single event slot.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`EventSlot`](vc64.md#eventslot) | [`slot`](#slot)  | The event slot. |
| [`EventID`](vc64.md#eventid) | [`eventId`](#eventid-1)  | Identifier of the scheduled event (0 if the slot is empty). |
| `const char *` | [`eventName`](#eventname)  | Name of the scheduled event. |
| [`Cycle`](vc64.md#cycle) | [`trigger`](#trigger)  | Trigger cycle of the event. |
| [`Cycle`](vc64.md#cycle) | [`triggerRel`](#triggerrel)  | Time until the event triggers (in cycles). |
| `long` | [`frameRel`](#framerel)  | Trigger frame relative to the current frame. |
| `long` | [`vpos`](#vpos-1)  | Raster line at which the event triggers. |
| `long` | [`hpos`](#hpos-1)  | Raster cycle at which the event triggers. |

---

### slot

```cpp
EventSlot slot
```

Type: [`EventSlot`](vc64.md#eventslot)

Defined in Components/C64Types.h:340

The event slot.

---

### eventId

```cpp
EventID eventId
```

Type: [`EventID`](vc64.md#eventid)

Defined in Components/C64Types.h:341

Identifier of the scheduled event (0 if the slot is empty).

---

### eventName

```cpp
const char * eventName
```

Defined in Components/C64Types.h:342

Name of the scheduled event.

---

### trigger

```cpp
Cycle trigger
```

Type: [`Cycle`](vc64.md#cycle)

Defined in Components/C64Types.h:345

Trigger cycle of the event.

---

### triggerRel

```cpp
Cycle triggerRel
```

Type: [`Cycle`](vc64.md#cycle)

Defined in Components/C64Types.h:346

Time until the event triggers (in cycles).

---

### frameRel

```cpp
long frameRel
```

Defined in Components/C64Types.h:349

Trigger frame relative to the current frame.

---

### vpos

```cpp
long vpos
```

Defined in Components/C64Types.h:352

Raster line at which the event triggers.

---

### hpos

```cpp
long hpos
```

Defined in Components/C64Types.h:353

Raster cycle at which the event triggers.

