---
title: "Memory"
---
# Memory

The memory and the memory debugger.

## Classes

| Name | Description |
|------|-------------|
| [`MemoryAPI`](vc64-MemoryAPI.md#memoryapi) | Memory [API](vc64-API.md#api) |
| [`MemoryDebuggerAPI`](vc64-MemoryDebuggerAPI.md#memorydebuggerapi) | Memory Debugger [API](vc64-API.md#api) |
| [`MemConfig`](vc64-MemConfig.md#memconfig) | Configuration of the memory. |
| [`MemInfo`](vc64-MemInfo.md#meminfo) | State of the memory. |
| [`MemMetrics`](vc64-MemMetrics.md#memmetrics) | Memory access statistics. |

## Enumerations

| Name | Description |
|------|-------------|
| [`MemAccess`](#memaccess)  | Types of VICII memory accesses. |
| [`MemType`](#memtype)  | Memory type. |
| [`RamPattern`](#rampattern)  | Ram startup pattern. |

---

### MemAccess

```cpp
enum MemAccess
```

Defined in Components/VICII/BusTypes.h:21

Types of VICII memory accesses.

| Value | Description |
|-------|-------------|
| `R` | Memory Refresh. |
| `I` | Idle read. |
| `C` | Character access. |
| `G` | Graphics access. |
| `P` | Sprite pointer access. |
| `S` | Sprite data access. |
| `COUNT` |  |

---

### MemType

```cpp
enum MemType
```

Defined in Components/Memory/MemoryTypes.h:42

Memory type.

| Value | Description |
|-------|-------------|
| `NONE` | Unmapped. |
| `RAM` | Ram. |
| `CHAR` | Character Rom. |
| `KERNAL` | Kernal Rom. |
| `BASIC` | Basic Rom. |
| `IO` | IO space. |
| `CRTLO` | Cartridge Rom (low bank). |
| `CRTHI` | Cartridge Rom (high bank). |
| `PP` | Processor port. |

---

### RamPattern

```cpp
enum RamPattern
```

Defined in Components/Memory/MemoryTypes.h:84

Ram startup pattern.

| Value | Description |
|-------|-------------|
| `VICE` | Pattern used by the VICE emulator. |
| `CCS` | Pattern used by the CCS emulator. |
| `ZEROES` | Initialize with all zeroes. |
| `ONES` | Initialize with all ones. |
| `RANDOM` | Initialize with pseudo-random values. |
