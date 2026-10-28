---
title: "SectorLayout"
kind: struct
---
# SectorLayout

```cpp
#include <Peripherals/Drive/DiskAnalyzerTypes.h>
```

```cpp
struct SectorLayout
```

Defined in Peripherals/Drive/DiskAnalyzerTypes.h:20

Position of a sector on a track (in bits).

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`headerBegin`](#headerbegin)  | Start of the sector header. |
| `isize` | [`headerEnd`](#headerend)  | End of the sector header. |
| `isize` | [`dataBegin`](#databegin)  | Start of the data block. |
| `isize` | [`dataEnd`](#dataend)  | End of the data block. |

---

### headerBegin

```cpp
isize headerBegin
```

Defined in Peripherals/Drive/DiskAnalyzerTypes.h:22

Start of the sector header.

---

### headerEnd

```cpp
isize headerEnd
```

Defined in Peripherals/Drive/DiskAnalyzerTypes.h:23

End of the sector header.

---

### dataBegin

```cpp
isize dataBegin
```

Defined in Peripherals/Drive/DiskAnalyzerTypes.h:24

Start of the data block.

---

### dataEnd

```cpp
isize dataEnd
```

Defined in Peripherals/Drive/DiskAnalyzerTypes.h:25

End of the data block.

