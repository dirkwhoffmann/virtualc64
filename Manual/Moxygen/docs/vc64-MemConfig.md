---
title: "MemConfig"
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
| [`RamPattern`](grp_memory.md#rampattern) | [`ramPattern`](#rampattern-1)  | RAM start-up pattern. |
| `bool` | [`saveRoms`](#saveroms)  | Include ROMs in snapshots. |
| `bool` | [`heatmap`](#heatmap)  | Count memory accesses for the heatmap. |

---

### ramPattern

```cpp
RamPattern ramPattern
```

Type: [`RamPattern`](grp_memory.md#rampattern)

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

