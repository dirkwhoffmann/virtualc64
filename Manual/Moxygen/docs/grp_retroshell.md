---
title: "RetroShell"
---
# RetroShell

The command console.

## Classes

| Name | Description |
|------|-------------|
| [`RetroShellAPI`](vc64-RetroShellAPI.md#retroshellapi) | RetroShell Public [API](vc64-API.md#api) |
| [`InputLine`](vc64-InputLine.md#inputline) | A line of input for RetroShell. |
| [`RetroShellInfo`](vc64-RetroShellInfo.md#retroshellinfo) | State of RetroShell. |
| [`vspace`](vc64-vspace.md#vspace) | Vertical spacing, used in operator overloads. |

## Enumerations

| Name | Description |
|------|-------------|
| [`CommandSet`](#commandset)  | [Command](vc64-Command.md#command) sets of RetroShell. |
| [`RSKey`](#rskey)  | RetroShell special key. |

---

### CommandSet

```cpp
enum CommandSet
```

Defined in Misc/RetroShell/RetroShellTypes.h:28

[Command](vc64-Command.md#command) sets of RetroShell.

| Value | Description |
|-------|-------------|
| `Commander` | Commands for controlling the emulator. |
| `Debugger` | Commands for debugging the emulator. |

---

### RSKey

```cpp
enum RSKey
```

Defined in Misc/RetroShell/RetroShellTypes.h:55

RetroShell special key.

| Value | Description |
|-------|-------------|
| `UP` |  |
| `DOWN` |  |
| `LEFT` |  |
| `RIGHT` |  |
| `PAGE_UP` |  |
| `PAGE_DOWN` |  |
| `DEL` |  |
| `CUT` |  |
| `BACKSPACE` |  |
| `HOME` |  |
| `END` |  |
| `TAB` |  |
| `RETURN` |  |
| `CR` |  |
