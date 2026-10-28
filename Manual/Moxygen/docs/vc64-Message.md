---
title: "Message"
---
# Message

```cpp
#include <Infrastructure/MsgQueueTypes.h>
```

```cpp
struct Message
```

Defined in Infrastructure/MsgQueueTypes.h:263

A message sent from the emulator to the GUI.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`Msg`](grp_messages.md#msg-1) | [`type`](#type-4)  | Type of the message. |
| `const char *` | [`str`](#str)  | Optional text (may be null). |

---

### type

```cpp
Msg type
```

Type: [`Msg`](grp_messages.md#msg-1)

Defined in Infrastructure/MsgQueueTypes.h:266

Type of the message.

---

### str

```cpp
const char * str
```

Defined in Infrastructure/MsgQueueTypes.h:279

Optional text (may be null).

