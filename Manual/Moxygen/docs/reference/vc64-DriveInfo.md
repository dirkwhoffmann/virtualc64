---
title: "DriveInfo"
kind: struct
---
# DriveInfo

```cpp
#include <Peripherals/Drive/DriveTypes.h>
```

```cpp
struct DriveInfo
```

Defined in Peripherals/Drive/DriveTypes.h:225

State of a drive.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`id`](#id-1)  | Number of the drive. |
| `bool` | [`hasRom`](#hasrom)  | A ROM is installed. |
| `bool` | [`hasDisk`](#hasdisk)  | A disk is inserted. |
| `bool` | [`hasUnprotectedDisk`](#hasunprotecteddisk)  | The inserted disk is not write-protected. |
| `bool` | [`hasProtectedDisk`](#hasprotecteddisk)  | The inserted disk is write-protected. |
| `bool` | [`hasUnmodifiedDisk`](#hasunmodifieddisk)  | The inserted disk has no unsaved changes. |
| `bool` | [`hasModifiedDisk`](#hasmodifieddisk)  | The inserted disk has unsaved changes. |
| `bool` | [`redLED`](#redled)  | The red LED is on. |
| `bool` | [`greenLED`](#greenled)  | The green LED is on. |
| `bool` | [`spinning`](#spinning)  | The disk is spinning. |
| `bool` | [`writing`](#writing)  | The drive is writing. |
| [`Halftrack`](/reference/vc64.md#halftrack) | [`halftrack`](#halftrack-1)  | Current head position (halftrack). |
| [`HeadPos`](/reference/vc64.md#headpos) | [`offset`](#offset)  | [Head](/reference/vc64.md#head) offset within the track. |

---

### id

```cpp
isize id
```

Defined in Peripherals/Drive/DriveTypes.h:227

Number of the drive.

---

### hasRom

```cpp
bool hasRom
```

Defined in Peripherals/Drive/DriveTypes.h:229

A ROM is installed.

---

### hasDisk

```cpp
bool hasDisk
```

Defined in Peripherals/Drive/DriveTypes.h:231

A disk is inserted.

---

### hasUnprotectedDisk

```cpp
bool hasUnprotectedDisk
```

Defined in Peripherals/Drive/DriveTypes.h:232

The inserted disk is not write-protected.

---

### hasProtectedDisk

```cpp
bool hasProtectedDisk
```

Defined in Peripherals/Drive/DriveTypes.h:233

The inserted disk is write-protected.

---

### hasUnmodifiedDisk

```cpp
bool hasUnmodifiedDisk
```

Defined in Peripherals/Drive/DriveTypes.h:234

The inserted disk has no unsaved changes.

---

### hasModifiedDisk

```cpp
bool hasModifiedDisk
```

Defined in Peripherals/Drive/DriveTypes.h:235

The inserted disk has unsaved changes.

---

### redLED

```cpp
bool redLED
```

Defined in Peripherals/Drive/DriveTypes.h:237

The red LED is on.

---

### greenLED

```cpp
bool greenLED
```

Defined in Peripherals/Drive/DriveTypes.h:238

The green LED is on.

---

### spinning

```cpp
bool spinning
```

Defined in Peripherals/Drive/DriveTypes.h:240

The disk is spinning.

---

### writing

```cpp
bool writing
```

Defined in Peripherals/Drive/DriveTypes.h:241

The drive is writing.

---

### halftrack

```cpp
Halftrack halftrack
```

Type: [`Halftrack`](/reference/vc64.md#halftrack)

Defined in Peripherals/Drive/DriveTypes.h:243

Current head position (halftrack).

---

### offset

```cpp
HeadPos offset
```

Type: [`HeadPos`](/reference/vc64.md#headpos)

Defined in Peripherals/Drive/DriveTypes.h:244

[Head](/reference/vc64.md#head) offset within the track.

