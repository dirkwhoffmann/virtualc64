---
title: "TODInfo"
---
# TODInfo

```cpp
#include <Components/CIA/TODTypes.h>
```

```cpp
struct TODInfo
```

Defined in Components/CIA/TODTypes.h:37

State of the time-of-day clock.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`TimeOfDay`](vc64-TimeOfDay.md#timeofday) | [`time`](#time)  | Current time. |
| [`TimeOfDay`](vc64-TimeOfDay.md#timeofday) | [`latch`](#latch-1)  | Latched time. |
| [`TimeOfDay`](vc64-TimeOfDay.md#timeofday) | [`alarm`](#alarm)  | Alarm time. |

---

### time

```cpp
TimeOfDay time
```

Type: [`TimeOfDay`](vc64-TimeOfDay.md#timeofday)

Defined in Components/CIA/TODTypes.h:39

Current time.

---

### latch

```cpp
TimeOfDay latch
```

Type: [`TimeOfDay`](vc64-TimeOfDay.md#timeofday)

Defined in Components/CIA/TODTypes.h:40

Latched time.

---

### alarm

```cpp
TimeOfDay alarm
```

Type: [`TimeOfDay`](vc64-TimeOfDay.md#timeofday)

Defined in Components/CIA/TODTypes.h:41

Alarm time.

