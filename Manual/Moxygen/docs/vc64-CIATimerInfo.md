---
title: "CIATimerInfo"
---
# CIATimerInfo

```cpp
#include <Components/CIA/CIATypes.h>
```

```cpp
struct CIATimerInfo
```

Defined in Components/CIA/CIATypes.h:130

State of a CIA timer.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `u16` | [`count`](#count)  | Current counter value. |
| `u16` | [`latch`](#latch)  | Latch value the counter is reloaded from. |
| `bool` | [`running`](#running)  | The timer is running. |
| `bool` | [`toggle`](#toggle)  | Toggle mode is on. |
| `bool` | [`pbout`](#pbout)  | The timer output appears on port B. |
| `bool` | [`oneShot`](#oneshot)  | One-shot mode is on. |

---

### count

```cpp
u16 count
```

Defined in Components/CIA/CIATypes.h:132

Current counter value.

---

### latch

```cpp
u16 latch
```

Defined in Components/CIA/CIATypes.h:133

Latch value the counter is reloaded from.

---

### running

```cpp
bool running
```

Defined in Components/CIA/CIATypes.h:134

The timer is running.

---

### toggle

```cpp
bool toggle
```

Defined in Components/CIA/CIATypes.h:135

Toggle mode is on.

---

### pbout

```cpp
bool pbout
```

Defined in Components/CIA/CIATypes.h:136

The timer output appears on port B.

---

### oneShot

```cpp
bool oneShot
```

Defined in Components/CIA/CIATypes.h:137

One-shot mode is on.

