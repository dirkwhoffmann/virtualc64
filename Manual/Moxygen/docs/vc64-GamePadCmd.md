---
title: "GamePadCmd"
kind: struct
---
# GamePadCmd

```cpp
#include <Infrastructure/CmdQueueTypes.h>
```

```cpp
struct GamePadCmd
```

Defined in Infrastructure/CmdQueueTypes.h:271

Payload of joystick commands.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`port`](#port-2)  | Control port number. |
| [`GamePadAction`](enum-GamePadAction.md) | [`action`](#action)  | The joystick action. |

---

### port

```cpp
isize port
```

Defined in Infrastructure/CmdQueueTypes.h:273

Control port number.

---

### action

```cpp
GamePadAction action
```

Type: [`GamePadAction`](enum-GamePadAction.md)

Defined in Infrastructure/CmdQueueTypes.h:274

The joystick action.

