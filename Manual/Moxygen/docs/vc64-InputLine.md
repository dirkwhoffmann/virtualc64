---
title: "InputLine"
kind: struct
---
# InputLine

```cpp
#include <Misc/RetroShell/RetroShellTypes.h>
```

```cpp
struct InputLine
```

Defined in Misc/RetroShell/RetroShellTypes.h:110

A line of input for RetroShell.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`id`](#id-2)  | Line number, RPC identifier, etc. |
| [`Source`](#source) | [`type`](#type-7)  | Indicates where the command comes from. |
| `string` | [`input`](#input)  | The command to execute. |
| [`Completion`](#completion-1) | [`completion`](#completion)  | Optional completion handler. |

---

### id

```cpp
isize id
```

Defined in Misc/RetroShell/RetroShellTypes.h:122

Line number, RPC identifier, etc.

---

### type

```cpp
Source type
```

Type: [`Source`](#source)

Defined in Misc/RetroShell/RetroShellTypes.h:125

Indicates where the command comes from.

---

### input

```cpp
string input
```

Defined in Misc/RetroShell/RetroShellTypes.h:128

The command to execute.

---

### completion

```cpp
Completion completion
```

Type: [`Completion`](#completion-1)

Defined in Misc/RetroShell/RetroShellTypes.h:141

Optional completion handler.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`isUserCommand`](#isusercommand) `const` `inline` | Returns true if the line was typed by the user. |
| `bool` | [`isScriptCommand`](#isscriptcommand) `const` `inline` | Returns true if the line stems from a script. |
| `bool` | [`isRpcCommand`](#isrpccommand) `const` `inline` | Returns true if the line is a JSON RPC request. |
| `bool` | [`isRshCommand`](#isrshcommand) `const` `inline` | Returns true if the line is a remote shell request. |

---

### isUserCommand

`const` `inline`

```cpp
inline bool isUserCommand() const
```

Defined in Misc/RetroShell/RetroShellTypes.h:144

Returns true if the line was typed by the user.

---

### isScriptCommand

`const` `inline`

```cpp
inline bool isScriptCommand() const
```

Defined in Misc/RetroShell/RetroShellTypes.h:146

Returns true if the line stems from a script.

---

### isRpcCommand

`const` `inline`

```cpp
inline bool isRpcCommand() const
```

Defined in Misc/RetroShell/RetroShellTypes.h:148

Returns true if the line is a JSON RPC request.

---

### isRshCommand

`const` `inline`

```cpp
inline bool isRshCommand() const
```

Defined in Misc/RetroShell/RetroShellTypes.h:150

Returns true if the line is a remote shell request.

## Public Types

| Name | Description |
|------|-------------|
| [`Source`](#source)  | Origin of an input line. |
| [`Completion`](#completion-1)  | Signature of the completion handler. |

---

### Source

```cpp
enum Source
```

Defined in Misc/RetroShell/RetroShellTypes.h:113

Origin of an input line.

| Value | Description |
|-------|-------------|
| `USER` | User-typed command. |
| `SCRIPT` | Script command. |
| `RPC` | JSON RPC request. |
| `RSH` | RemoteShell request. |

---

### Completion

```cpp
using Completion = std::function< void(const string &output, std::exception_ptr error)>
```

Defined in Misc/RetroShell/RetroShellTypes.h:140

Signature of the completion handler.

