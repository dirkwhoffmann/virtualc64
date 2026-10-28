---
title: "Guard"
---
# Guard

```cpp
#include <Components/CPU/Peddle/PeddleDebuggerTypes.h>
```

```cpp
struct Guard
```

Defined in Components/CPU/Peddle/PeddleDebuggerTypes.h:15

Base structure for a single breakpoint or watchpoint.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `u32` | [`addr`](#addr)  | The observed address. |
| `bool` | [`enabled`](#enabled)  | Disabled guards never trigger. |
| `isize` | [`hits`](#hits)  | Counts the number of hits. |
| `isize` | [`ignore`](#ignore)  | Ignore counter. |

---

### addr

```cpp
u32 addr
```

Defined in Components/CPU/Peddle/PeddleDebuggerTypes.h:18

The observed address.

---

### enabled

```cpp
bool enabled
```

Defined in Components/CPU/Peddle/PeddleDebuggerTypes.h:21

Disabled guards never trigger.

---

### hits

```cpp
isize hits
```

Defined in Components/CPU/Peddle/PeddleDebuggerTypes.h:24

Counts the number of hits.

---

### ignore

```cpp
isize ignore
```

Defined in Components/CPU/Peddle/PeddleDebuggerTypes.h:27

Ignore counter.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`eval`](#eval)  | Returns true if the guard hits. |
| `void` | [`moveTo`](#moveto)  | Replaces the address by another. |

---

### eval

```cpp
bool eval(u32 addr)
```

Defined in Components/CPU/Peddle/PeddleDebuggerTypes.h:32

Returns true if the guard hits.

---

### moveTo

```cpp
void moveTo(u32 newAddr)
```

Defined in Components/CPU/Peddle/PeddleDebuggerTypes.h:35

Replaces the address by another.

