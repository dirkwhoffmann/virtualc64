---
title: "GuardCmd"
---
# GuardCmd

```cpp
#include <Infrastructure/CmdQueueTypes.h>
```

```cpp
struct GuardCmd
```

Defined in Infrastructure/CmdQueueTypes.h:303

Payload of breakpoint and watchpoint commands.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `GuardType` | [`list`](#list)  | The guard list to modify. |
| `isize` | [`nr`](#nr-1)  | Number of a guard in the list. |
| `u32` | [`addr`](#addr-1)  | Target of a guard. |
| `isize` | [`ignores`](#ignores)  | Ignore count. |

---

### list

```cpp
GuardType list
```

Defined in Infrastructure/CmdQueueTypes.h:305

The guard list to modify.

---

### nr

```cpp
isize nr
```

Defined in Infrastructure/CmdQueueTypes.h:306

Number of a guard in the list.

---

### addr

```cpp
u32 addr
```

Defined in Infrastructure/CmdQueueTypes.h:307

Target of a guard.

---

### ignores

```cpp
isize ignores
```

Defined in Infrastructure/CmdQueueTypes.h:308

Ignore count.

