---
title: "TrackDefaults"
---
# TrackDefaults

```cpp
#include <Peripherals/Drive/FloppyDiskTypes.h>
```

```cpp
struct TrackDefaults
```

Defined in Peripherals/Drive/FloppyDiskTypes.h:175

Default properties of a track.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `u8` | [`sectors`](#sectors)  | Typical number of sectors in this track. |
| `u8` | [`speedZone`](#speedzone)  | Default speed zone for this track. |
| `u16` | [`lengthInBytes`](#lengthinbytes)  | Typical track size in bits. |
| `u16` | [`lengthInBits`](#lengthinbits)  | Typical track size in bits. |
| [`Sector`](vc64.md#sector) | [`firstSectorNr`](#firstsectornr)  | Logical number of first sector in track. |
| `double` | [`stagger`](#stagger)  | Relative position of first bit (from Hoxs64). |

---

### sectors

```cpp
u8 sectors
```

Defined in Peripherals/Drive/FloppyDiskTypes.h:177

Typical number of sectors in this track.

---

### speedZone

```cpp
u8 speedZone
```

Defined in Peripherals/Drive/FloppyDiskTypes.h:178

Default speed zone for this track.

---

### lengthInBytes

```cpp
u16 lengthInBytes
```

Defined in Peripherals/Drive/FloppyDiskTypes.h:179

Typical track size in bits.

---

### lengthInBits

```cpp
u16 lengthInBits
```

Defined in Peripherals/Drive/FloppyDiskTypes.h:180

Typical track size in bits.

---

### firstSectorNr

```cpp
Sector firstSectorNr
```

Type: [`Sector`](vc64.md#sector)

Defined in Peripherals/Drive/FloppyDiskTypes.h:181

Logical number of first sector in track.

---

### stagger

```cpp
double stagger
```

Defined in Peripherals/Drive/FloppyDiskTypes.h:182

Relative position of first bit (from Hoxs64).

