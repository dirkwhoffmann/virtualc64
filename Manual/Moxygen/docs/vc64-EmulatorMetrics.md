---
title: "EmulatorMetrics"
---
# EmulatorMetrics

```cpp
#include <Infrastructure/EmulatorTypes.h>
```

```cpp
struct EmulatorMetrics
```

Defined in Infrastructure/EmulatorTypes.h:331

Collected run-time data.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `double` | [`cpuLoad`](#cpuload)  | Measured CPU load. |
| `double` | [`fps`](#fps-1)  | Measured frames per seconds. |
| `isize` | [`resyncs`](#resyncs)  | Number of out-of-sync conditions. |
| `isize` | [`clones`](#clones)  | Number of created run-ahead instances. |

---

### cpuLoad

```cpp
double cpuLoad
```

Defined in Infrastructure/EmulatorTypes.h:333

Measured CPU load.

---

### fps

```cpp
double fps
```

Defined in Infrastructure/EmulatorTypes.h:334

Measured frames per seconds.

---

### resyncs

```cpp
isize resyncs
```

Defined in Infrastructure/EmulatorTypes.h:335

Number of out-of-sync conditions.

---

### clones

```cpp
isize clones
```

Defined in Infrastructure/EmulatorTypes.h:336

Number of created run-ahead instances.

