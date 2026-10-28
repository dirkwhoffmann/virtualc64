---
title: "CPUDebuggerAPI"
kind: class
---
# CPUDebuggerAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class CPUDebuggerAPI
```

Defined in VirtualC64.h:366

CPU Debugger [API](/reference/vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`loggedInstructions`](#loggedinstructions) `const` | Returns the number of instructions in the record buffer. |
| `void` | [`clearLog`](#clearlog)  | Empties the record buffer. |
| `RecordedInstruction` | [`logEntry`](#logentry) `const` | Returns an entry from the record buffer. Returned by value (rather than by reference) since the log buffer is written concurrently by the emulator thread. |
| `void` | [`setNumberFormat`](#setnumberformat)  | Determines how the disassembler displays numbers. |
| `string` | [`disassemble`](#disassemble) `const` | Disassembles an instruction. |
| `string` | [`disassembleRecorded`](#disassemblerecorded) `const` | Disassembles an instruction from the record buffer. |

---

### loggedInstructions

`const`

```cpp
isize loggedInstructions() const
```

Defined in VirtualC64.h:380

Returns the number of instructions in the record buffer.

> [!NOTE]
> The record buffer is only filled in track mode. To save computation time, the GUI enables track mode when the CPU inspector is opened and disables track mode when it is closed.

---

### clearLog

```cpp
void clearLog()
```

Defined in VirtualC64.h:384

Empties the record buffer.

---

### logEntry

`const`

```cpp
RecordedInstruction logEntry(isize nr) const
```

Defined in VirtualC64.h:393

Returns an entry from the record buffer. Returned by value (rather than by reference) since the log buffer is written concurrently by the emulator thread. 
#### Returns
The recorded instruction, including the address (pc) it was fetched from.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | Index of the instruction in the record buffer |

---

### setNumberFormat

```cpp
void setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat)
```

Defined in VirtualC64.h:399

Determines how the disassembler displays numbers.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `instrFormat` | `DasmNumberFormat` | Format for numbers inside instructions |
| `dataFormat` | `DasmNumberFormat` | Format for printed data values |

---

### disassemble

`const`

```cpp
string disassemble(u16 addr, const string & fmt, isize * len = nullptr) const
```

Defined in VirtualC64.h:409

Disassembles an instruction. 
#### Returns
The disassembled text.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `u16` | Address of the instruction to disassemble |
| `fmt` | `const string &` | String defining the output format. The directives are p (address), b (instruction bytes), i (instruction), and f (flags, recorded instructions only). |
| `len` | `isize *` | If given, receives the length of the instruction in bytes. |

---

### disassembleRecorded

`const`

```cpp
string disassembleRecorded(isize nr, const string & fmt, isize * len = nullptr) const
```

Defined in VirtualC64.h:417

Disassembles an instruction from the record buffer. 
#### Returns
The disassembled text.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | Index of the instruction in the record buffer |
| `fmt` | `const string &` | String defining the output format (see [disassemble()](#disassemble)) |
| `len` | `isize *` | If given, receives the length of the instruction in bytes. |

