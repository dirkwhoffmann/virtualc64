---
title: "TrackLayout"
kind: struct
---
# TrackLayout

```cpp
#include <Peripherals/Drive/DiskAnalyzerTypes.h>
```

```cpp
struct TrackLayout
```

Defined in Peripherals/Drive/DiskAnalyzerTypes.h:30

Layout of a track.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`length`](#length)  | Length of the track (in bits). |
| [`SectorLayout`](vc64-SectorLayout.md#sectorlayout) | [`sectorLayout`](#sectorlayout-1)  | Layout of the sectors. |

---

### length

```cpp
isize length
```

Defined in Peripherals/Drive/DiskAnalyzerTypes.h:32

Length of the track (in bits).

---

### sectorLayout

```cpp
SectorLayout sectorLayout[22]
```

Defined in Peripherals/Drive/DiskAnalyzerTypes.h:33

Layout of the sectors.

