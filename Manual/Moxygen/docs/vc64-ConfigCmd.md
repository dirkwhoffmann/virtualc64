---
title: "ConfigCmd"
kind: struct
---
# ConfigCmd

```cpp
#include <Infrastructure/CmdQueueTypes.h>
```

```cpp
struct ConfigCmd
```

Defined in Infrastructure/CmdQueueTypes.h:245

Payload of configuration commands.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`Opt`](enum-Opt.md) | [`option`](#option)  | The option to change. |
| `i64` | [`value`](#value-1)  | The new value. |
| `isize` | [`id`](#id)  | The component the option belongs to. |

---

### option

```cpp
Opt option
```

Type: [`Opt`](enum-Opt.md)

Defined in Infrastructure/CmdQueueTypes.h:247

The option to change.

---

### value

```cpp
i64 value
```

Defined in Infrastructure/CmdQueueTypes.h:248

The new value.

---

### id

```cpp
isize id
```

Defined in Infrastructure/CmdQueueTypes.h:249

The component the option belongs to.

