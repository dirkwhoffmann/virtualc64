---
title: "GuardInfo"
kind: struct
---
# GuardInfo

```cpp
#include <Components/CPU/CPUTypes.h>
```

```cpp
struct GuardInfo
```

Defined in Components/CPU/CPUTypes.h:158

Description of a breakpoint or watchpoint.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `u32` | [`addr`](#addr-2)  | Observed address. |
| `bool` | [`enabled`](#enabled-1)  | The guard is enabled. |
| `isize` | [`hits`](#hits-1)  | Number of times the guard was hit. |
| `isize` | [`ignore`](#ignore-1)  | Number of hits that are ignored before the guard triggers. |

---

### addr

```cpp
u32 addr
```

Defined in Components/CPU/CPUTypes.h:160

Observed address.

---

### enabled

```cpp
bool enabled
```

Defined in Components/CPU/CPUTypes.h:161

The guard is enabled.

---

### hits

```cpp
isize hits
```

Defined in Components/CPU/CPUTypes.h:162

Number of times the guard was hit.

---

### ignore

```cpp
isize ignore
```

Defined in Components/CPU/CPUTypes.h:163

Number of hits that are ignored before the guard triggers.

