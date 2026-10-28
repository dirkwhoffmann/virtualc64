---
title: "Message"
kind: struct
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
| [`Msg`](enum-Msg.md) | [`type`](#type-8)  | Type of the message. |
| `const char *` | [`str`](#str-1)  | Optional text (may be null). |

---

### type

```cpp
Msg type
```

Type: [`Msg`](enum-Msg.md)

Defined in Infrastructure/MsgQueueTypes.h:266

Type of the message.

---

### str

```cpp
const char * str
```

Defined in Infrastructure/MsgQueueTypes.h:279

Optional text (may be null).

