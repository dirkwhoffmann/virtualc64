---
title: "AlarmCmd"
kind: struct
---
# AlarmCmd

```cpp
#include <Infrastructure/CmdQueueTypes.h>
```

```cpp
struct AlarmCmd
```

Defined in Infrastructure/CmdQueueTypes.h:294

Payload of alarm commands.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `i64` | [`cycle`](#cycle-1)  | [Cycle](/reference/vc64.md#cycle) at which the alarm fires. |
| `i64` | [`value`](#value)  | Value reported by the alarm. |

---

### cycle

```cpp
i64 cycle
```

Defined in Infrastructure/CmdQueueTypes.h:296

[Cycle](/reference/vc64.md#cycle) at which the alarm fires.

---

### value

```cpp
i64 value
```

Defined in Infrastructure/CmdQueueTypes.h:297

Value reported by the alarm.

