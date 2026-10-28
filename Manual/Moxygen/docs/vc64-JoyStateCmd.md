---
title: "JoyStateCmd"
---
# JoyStateCmd

```cpp
#include <Infrastructure/CmdQueueTypes.h>
```

```cpp
struct JoyStateCmd
```

Defined in Infrastructure/CmdQueueTypes.h:279

Payload of joystick state commands.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`port`](#port-1)  | Control port number. |
| `u32` | [`state`](#state-1)  | Bits 0-4: current state, bits 8-12: previous state (up, down, left, right, fire). |

---

### port

```cpp
isize port
```

Defined in Infrastructure/CmdQueueTypes.h:281

Control port number.

---

### state

```cpp
u32 state
```

Defined in Infrastructure/CmdQueueTypes.h:282

Bits 0-4: current state, bits 8-12: previous state (up, down, left, right, fire).

