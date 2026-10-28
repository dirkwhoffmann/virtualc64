---
title: "MemoryDebuggerAPI"
kind: class
---
# MemoryDebuggerAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class MemoryDebuggerAPI
```

Defined in VirtualC64.h:130

Memory Debugger [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `string` | [`ascDump`](#ascdump) `const` | Returns a string of characters for a portion of memory. |
| `string` | [`hexDump`](#hexdump) `const` | Returns a string of numbers for a portion of memory. |
| `string` | [`decDump`](#decdump) `const` | Variant of [hexDump()](#hexdump) that uses a different output format. |
| `string` | [`memDump`](#memdump) `const` | Variant of [hexDump()](#hexdump) that uses a different output format. |

---

### ascDump

`const`

```cpp
string ascDump(MemType src, u16 addr, isize bytes) const
```

Defined in VirtualC64.h:149

Returns a string of characters for a portion of memory.

Unmapped memory ([MemType::NONE](enum-MemType.md)) is shown as nothing.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | [`MemType`](enum-MemType.md) | The memory type the bytes are read from. |
| `addr` | `u16` | First address to dump. |
| `bytes` | `isize` | Number of bytes to dump. |

---

### hexDump

`const`

```cpp
string hexDump(MemType src, u16 addr, isize bytes, isize sz = 1) const
```

Defined in VirtualC64.h:163

Returns a string of numbers for a portion of memory.

[hexDump()](#hexdump) shows the bytes as hexadecimal numbers, [decDump()](#decdump) as decimal numbers, and [memDump()](#memdump) as hexadecimal numbers followed by characters. Each group of 'sz' bytes forms one number and is followed by a space. Unmapped memory ([MemType::NONE](enum-MemType.md)) is shown as '&ndash;'.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | [`MemType`](enum-MemType.md) | The memory type the bytes are read from. |
| `addr` | `u16` | First address to dump. |
| `bytes` | `isize` | Number of bytes to dump. |
| `sz` | `isize` | Group size in bytes (1, 2, or 4). |

---

### decDump

`const`

```cpp
string decDump(MemType src, u16 addr, isize bytes, isize sz = 1) const
```

Defined in VirtualC64.h:165

Variant of [hexDump()](#hexdump) that uses a different output format.

---

### memDump

`const`

```cpp
string memDump(MemType src, u16 addr, isize bytes, isize sz = 1) const
```

Defined in VirtualC64.h:167

Variant of [hexDump()](#hexdump) that uses a different output format.

