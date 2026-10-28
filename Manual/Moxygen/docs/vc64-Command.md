---
title: "Command"
---
# Command

```cpp
#include <Infrastructure/CmdQueueTypes.h>
```

```cpp
struct Command
```

Defined in Infrastructure/CmdQueueTypes.h:313

A command that is fed into the command queue.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`Cmd`](grp_commands.md#cmd) | [`type`](#type-5)  | Type of the command. |
| `void *` | [`sender`](#sender)  | Sender of the command (optional). |
| `const char *` | [`str`](#str-1)  | Attached string (may be null). |

---

### type

```cpp
Cmd type
```

Type: [`Cmd`](grp_commands.md#cmd)

Defined in Infrastructure/CmdQueueTypes.h:316

Type of the command.

---

### sender

```cpp
void * sender
```

Defined in Infrastructure/CmdQueueTypes.h:319

Sender of the command (optional).

---

### str

```cpp
const char * str = nullptr
```

Defined in Infrastructure/CmdQueueTypes.h:339

Attached string (may be null).

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Command`](#command-1) `inline` | Creates an empty command. |
|  | [`Command`](#command-2) `inline` | Creates a command with two integer values. |
|  | [`Command`](#command-3) `inline` | Creates a command with a sender. |
|  | [`Command`](#command-4) `inline` | Creates a command with an alarm payload. |
|  | [`Command`](#command-5) `inline` | Creates a command with a configuration payload. |
|  | [`Command`](#command-6) `inline` | Creates a command with a coordinate payload. |
|  | [`Command`](#command-7) `inline` | Creates a command with a joystick payload. |
|  | [`Command`](#command-8) `inline` | Creates a command with a guard payload. |
|  | [`Command`](#command-9) `inline` | Creates a command with a joystick state payload. |
|  | [`Command`](#command-10) `inline` | Creates a command with a keyboard payload. |
|  | [`Command`](#command-11) `inline` | Creates a command with a tape payload. |

---

### Command

`inline`

```cpp
inline Command()
```

Defined in Infrastructure/CmdQueueTypes.h:342

Creates an empty command.

---

### Command

`inline`

```cpp
inline Command(Cmd type, i64 v1 = 0, i64 v2 = 0)
```

Defined in Infrastructure/CmdQueueTypes.h:344

Creates a command with two integer values.

---

### Command

`inline`

```cpp
inline Command(Cmd type, void * s, i64 v1 = 0, i64 v2 = 0)
```

Defined in Infrastructure/CmdQueueTypes.h:346

Creates a command with a sender.

---

### Command

`inline`

```cpp
inline Command(Cmd type, const AlarmCmd & cmd)
```

Defined in Infrastructure/CmdQueueTypes.h:348

Creates a command with an alarm payload.

---

### Command

`inline`

```cpp
inline Command(Cmd type, const ConfigCmd & cmd)
```

Defined in Infrastructure/CmdQueueTypes.h:350

Creates a command with a configuration payload.

---

### Command

`inline`

```cpp
inline Command(Cmd type, const CoordCmd & cmd)
```

Defined in Infrastructure/CmdQueueTypes.h:352

Creates a command with a coordinate payload.

---

### Command

`inline`

```cpp
inline Command(Cmd type, const GamePadCmd & cmd)
```

Defined in Infrastructure/CmdQueueTypes.h:354

Creates a command with a joystick payload.

---

### Command

`inline`

```cpp
inline Command(Cmd type, const GuardCmd & cmd)
```

Defined in Infrastructure/CmdQueueTypes.h:356

Creates a command with a guard payload.

---

### Command

`inline`

```cpp
inline Command(Cmd type, const JoyStateCmd & cmd)
```

Defined in Infrastructure/CmdQueueTypes.h:358

Creates a command with a joystick state payload.

---

### Command

`inline`

```cpp
inline Command(Cmd type, const KeyCmd & cmd)
```

Defined in Infrastructure/CmdQueueTypes.h:360

Creates a command with a keyboard payload.

---

### Command

`inline`

```cpp
inline Command(Cmd type, const TapeCmd & cmd)
```

Defined in Infrastructure/CmdQueueTypes.h:362

Creates a command with a tape payload.

