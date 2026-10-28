---
title: "EmulatorInfo"
kind: struct
---
# EmulatorInfo

```cpp
#include <Infrastructure/EmulatorTypes.h>
```

```cpp
struct EmulatorInfo
```

Defined in Infrastructure/EmulatorTypes.h:318

The current emulator state.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`ExecState`](/reference/enum-ExecState.md) | [`state`](#state)  | The current emulator state. |
| `bool` | [`powered`](#powered)  | Indicates if the emulator is powered on. |
| `bool` | [`paused`](#paused)  | Indicates if emulation is paused. |
| `bool` | [`running`](#running-2)  | Indicates if the emulator is running. |
| `bool` | [`suspended`](#suspended)  | Indicates if the emulator is in suspended state. |
| `bool` | [`warping`](#warping)  | Indicates if warp mode is currently on. |
| `bool` | [`tracking`](#tracking)  | Indicates if track mode is enabled. |

---

### state

```cpp
ExecState state
```

Type: [`ExecState`](/reference/enum-ExecState.md)

Defined in Infrastructure/EmulatorTypes.h:320

The current emulator state.

---

### powered

```cpp
bool powered
```

Defined in Infrastructure/EmulatorTypes.h:321

Indicates if the emulator is powered on.

---

### paused

```cpp
bool paused
```

Defined in Infrastructure/EmulatorTypes.h:322

Indicates if emulation is paused.

---

### running

```cpp
bool running
```

Defined in Infrastructure/EmulatorTypes.h:323

Indicates if the emulator is running.

---

### suspended

```cpp
bool suspended
```

Defined in Infrastructure/EmulatorTypes.h:324

Indicates if the emulator is in suspended state.

---

### warping

```cpp
bool warping
```

Defined in Infrastructure/EmulatorTypes.h:325

Indicates if warp mode is currently on.

---

### tracking

```cpp
bool tracking
```

Defined in Infrastructure/EmulatorTypes.h:326

Indicates if track mode is enabled.

