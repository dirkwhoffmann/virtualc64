---
title: "MemMetrics"
---
# MemMetrics

```cpp
#include <Components/Memory/MemoryTypes.h>
```

```cpp
struct MemMetrics
```

Defined in Components/Memory/MemoryTypes.h:146

Memory access statistics.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`reads`](#reads)  | Number of read accesses per address. |
| `isize` | [`writes`](#writes)  | Number of write accesses per address. |

---

### reads

```cpp
isize reads[65536]
```

Defined in Components/Memory/MemoryTypes.h:148

Number of read accesses per address.

---

### writes

```cpp
isize writes[65536]
```

Defined in Components/Memory/MemoryTypes.h:149

Number of write accesses per address.

