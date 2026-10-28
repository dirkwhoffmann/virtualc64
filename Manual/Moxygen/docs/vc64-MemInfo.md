---
title: "MemInfo"
---
# MemInfo

```cpp
#include <Components/Memory/MemoryTypes.h>
```

```cpp
struct MemInfo
```

Defined in Components/Memory/MemoryTypes.h:131

State of the memory.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`exrom`](#exrom)  | State of the EXROM line. |
| `bool` | [`game`](#game)  | State of the GAME line. |
| `bool` | [`loram`](#loram)  | LORAM bit of the processor port. |
| `bool` | [`hiram`](#hiram)  | HIRAM bit of the processor port. |
| `bool` | [`charen`](#charen)  | CHAREN bit of the processor port. |
| `u8` | [`bankMap`](#bankmap)  | Current memory bank configuration. |
| [`MemType`](vc64.md#memtype) | [`peekSrc`](#peeksrc)  | Memory type read by the CPU from each 4 KB bank. |
| [`MemType`](vc64.md#memtype) | [`vicPeekSrc`](#vicpeeksrc)  | Memory type read by the VICII from each 4 KB bank. |

---

### exrom

```cpp
bool exrom
```

Defined in Components/Memory/MemoryTypes.h:133

State of the EXROM line.

---

### game

```cpp
bool game
```

Defined in Components/Memory/MemoryTypes.h:134

State of the GAME line.

---

### loram

```cpp
bool loram
```

Defined in Components/Memory/MemoryTypes.h:135

LORAM bit of the processor port.

---

### hiram

```cpp
bool hiram
```

Defined in Components/Memory/MemoryTypes.h:136

HIRAM bit of the processor port.

---

### charen

```cpp
bool charen
```

Defined in Components/Memory/MemoryTypes.h:137

CHAREN bit of the processor port.

---

### bankMap

```cpp
u8 bankMap
```

Defined in Components/Memory/MemoryTypes.h:138

Current memory bank configuration.

---

### peekSrc

```cpp
MemType peekSrc[16]
```

Defined in Components/Memory/MemoryTypes.h:140

Memory type read by the CPU from each 4 KB bank.

---

### vicPeekSrc

```cpp
MemType vicPeekSrc[16]
```

Defined in Components/Memory/MemoryTypes.h:141

Memory type read by the VICII from each 4 KB bank.

