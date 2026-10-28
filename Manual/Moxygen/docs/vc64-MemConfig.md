---
title: "MemConfig"
kind: struct
---
# MemConfig

```cpp
#include <Components/Memory/MemoryTypes.h>
```

```cpp
struct MemConfig
```

Defined in Components/Memory/MemoryTypes.h:122

Configuration of the memory.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`RamPattern`](enum-RamPattern.md) | [`ramPattern`](#rampattern-1)  | RAM start-up pattern. |
| `bool` | [`saveRoms`](#saveroms-1)  | Include ROMs in snapshots. |
| `bool` | [`heatmap`](#heatmap)  | Count memory accesses for the heatmap. |

---

### ramPattern

```cpp
RamPattern ramPattern
```

Type: [`RamPattern`](enum-RamPattern.md)

Defined in Components/Memory/MemoryTypes.h:124

RAM start-up pattern.

---

### saveRoms

```cpp
bool saveRoms
```

Defined in Components/Memory/MemoryTypes.h:125

Include ROMs in snapshots.

---

### heatmap

```cpp
bool heatmap
```

Defined in Components/Memory/MemoryTypes.h:126

Count memory accesses for the heatmap.

