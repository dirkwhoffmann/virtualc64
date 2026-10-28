---
title: "CIAConfig"
---
# CIAConfig

```cpp
#include <Components/CIA/CIATypes.h>
```

```cpp
struct CIAConfig
```

Defined in Components/CIA/CIATypes.h:112

Configuration of a CIA chip.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`CIARev`](grp_cia.md#ciarev) | [`revision`](#revision-1)  | Chip revision. |
| `bool` | [`timerBBug`](#timerbbug)  | Emulate the Timer-B bug. |
| `bool` | [`idleSleep`](#idlesleep)  | Enter the idle state while the chip is not in use. |

---

### revision

```cpp
CIARev revision
```

Type: [`CIARev`](grp_cia.md#ciarev)

Defined in Components/CIA/CIATypes.h:114

Chip revision.

---

### timerBBug

```cpp
bool timerBBug
```

Defined in Components/CIA/CIATypes.h:115

Emulate the Timer-B bug.

---

### idleSleep

```cpp
bool idleSleep
```

Defined in Components/CIA/CIATypes.h:116

Enter the idle state while the chip is not in use.

