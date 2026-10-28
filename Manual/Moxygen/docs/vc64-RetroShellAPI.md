---
title: "RetroShellAPI"
---
# RetroShellAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class RetroShellAPI
```

Defined in VirtualC64.h:1426

RetroShell Public [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`RetroShellInfo`](vc64-RetroShellInfo.md#retroshellinfo) & | [`getInfo`](#getinfo-11) `const` | Returns the component's current state. |
| const [`RetroShellInfo`](vc64-RetroShellInfo.md#retroshellinfo) & | [`getCachedInfo`](#getcachedinfo-11) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-11)). |
| `string` | [`text`](#text) `const` | Returns a copy of the text buffer. The text buffer contains the complete contents of the console. It will be expanded when new output is generated. When the buffer grows too large, old contents is cropped. |
| `void` | [`press`](#press-1)  | Informs RetroShell that a key has been typed. |
| `void` | [`press`](#press-2)  | Informs RetroShell that a key has been typed. |
| `void` | [`press`](#press-3)  | Informs RetroShell that multiple keys have been typed. |
| `void` | [`setStream`](#setstream)  | Assign an additional output stream. In addition to writing the RetroShell output into the text buffer, RetroShell will write the output into the provides stream. |
| `void` | [`execScriptAsync`](#execscriptasync)  | Executes a script. The script is executed asynchronously. However, RetroShell will send messages back to the GUI thread to inform about the execution state. After the last pending command has been executed, [Msg::RSH_DONE](enum-Msg.md) is sent. If a command fails, the remaining commands are dropped and [Msg::RSH_ERROR](enum-Msg.md) is sent instead. A 'wait' command postpones the rest of the script and sends [Msg::RSH_WAIT](enum-Msg.md). |
| `void` | [`execScriptAsync`](#execscriptasync-1)  | Executes a script that is read from a string stream. |
| `void` | [`execScriptAsync`](#execscriptasync-2)  | Executes a script that is read from a file stream. |
| `void` | [`execScriptAsync`](#execscriptasync-3)  | Executes a script that is given as a string. |

---

### getInfo

`const`

```cpp
const RetroShellInfo & getInfo() const
```

Defined in VirtualC64.h:1446

Returns the component's current state.

[getInfo()](#getinfo-11) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-11) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const RetroShellInfo & getCachedInfo() const
```

Defined in VirtualC64.h:1448

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-11)).

---

### text

`const`

```cpp
string text() const
```

Defined in VirtualC64.h:1455

Returns a copy of the text buffer. The text buffer contains the complete contents of the console. It will be expanded when new output is generated. When the buffer grows too large, old contents is cropped.

---

### press

```cpp
void press(RSKey key, bool shift = false)
```

Defined in VirtualC64.h:1474

Informs RetroShell that a key has been typed.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | [`RSKey`](enum-RSKey.md) | The pressed key |
| `shift` | `bool` | Status of the shift key |

---

### press

```cpp
void press(char c)
```

Defined in VirtualC64.h:1479

Informs RetroShell that a key has been typed.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `c` | `char` | The pressed key |

---

### press

```cpp
void press(const string & s)
```

Defined in VirtualC64.h:1484

Informs RetroShell that multiple keys have been typed.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `s` | `const string &` | The typed text |

---

### setStream

```cpp
void setStream(std::ostream & os)
```

Defined in VirtualC64.h:1494

Assign an additional output stream. In addition to writing the RetroShell output into the text buffer, RetroShell will write the output into the provides stream.

---

### execScriptAsync

```cpp
void execScriptAsync(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1510

Executes a script. The script is executed asynchronously. However, RetroShell will send messages back to the GUI thread to inform about the execution state. After the last pending command has been executed, [Msg::RSH_DONE](enum-Msg.md) is sent. If a command fails, the remaining commands are dropped and [Msg::RSH_ERROR](enum-Msg.md) is sent instead. A 'wait' command postpones the rest of the script and sends [Msg::RSH_WAIT](enum-Msg.md).

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be opened. |

---

### execScriptAsync

```cpp
void execScriptAsync(std::stringstream & ss)
```

Defined in VirtualC64.h:1512

Executes a script that is read from a string stream.

---

### execScriptAsync

```cpp
void execScriptAsync(const std::ifstream & fs)
```

Defined in VirtualC64.h:1514

Executes a script that is read from a file stream.

---

### execScriptAsync

```cpp
void execScriptAsync(const string & contents)
```

Defined in VirtualC64.h:1516

Executes a script that is given as a string.

