---
title: "CIAPortInfo"
kind: struct
---
# CIAPortInfo

```cpp
#include <Components/CIA/CIATypes.h>
```

```cpp
struct CIAPortInfo
```

Defined in Components/CIA/CIATypes.h:121

State of a CIA port.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `u8` | [`port`](#port)  | State of the port pins. |
| `u8` | [`reg`](#reg)  | Port register (PRA or PRB). |
| `u8` | [`dir`](#dir)  | Data direction register (DDRA or DDRB). |

---

### port

```cpp
u8 port
```

Defined in Components/CIA/CIATypes.h:123

State of the port pins.

---

### reg

```cpp
u8 reg
```

Defined in Components/CIA/CIATypes.h:124

Port register (PRA or PRB).

---

### dir

```cpp
u8 dir
```

Defined in Components/CIA/CIATypes.h:125

Data direction register (DDRA or DDRB).

