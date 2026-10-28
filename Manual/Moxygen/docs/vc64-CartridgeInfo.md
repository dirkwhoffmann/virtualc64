---
title: "CartridgeInfo"
kind: struct
---
# CartridgeInfo

```cpp
#include <Media/Cartridges/CartridgeTypes.h>
```

```cpp
struct CartridgeInfo
```

Defined in Media/Cartridges/CartridgeTypes.h:288

State of an attached cartridge.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`CartridgeType`](enum-CartridgeType.md) | [`type`](#type-2)  | DEPRECATED. |
| `bool` | [`supported`](#supported)  | The cartridge type is supported. |
| `bool` | [`gameLineInCrtFile`](#gamelineincrtfile)  | Value of the GAME line stored in the CRT file. |
| `bool` | [`exromLineInCrtFile`](#exromlineincrtfile)  | Value of the EXROM line stored in the CRT file. |
| `isize` | [`numPackets`](#numpackets)  | Number of chip packets. |
| `isize` | [`switchPos`](#switchpos)  | Position of the switch. |
| `bool` | [`led`](#led)  | The LED is on. |

---

### type

```cpp
CartridgeType type
```

Type: [`CartridgeType`](enum-CartridgeType.md)

Defined in Media/Cartridges/CartridgeTypes.h:290

DEPRECATED.

---

### supported

```cpp
bool supported
```

Defined in Media/Cartridges/CartridgeTypes.h:291

The cartridge type is supported.

---

### gameLineInCrtFile

```cpp
bool gameLineInCrtFile
```

Defined in Media/Cartridges/CartridgeTypes.h:292

Value of the GAME line stored in the CRT file.

---

### exromLineInCrtFile

```cpp
bool exromLineInCrtFile
```

Defined in Media/Cartridges/CartridgeTypes.h:293

Value of the EXROM line stored in the CRT file.

---

### numPackets

```cpp
isize numPackets
```

Defined in Media/Cartridges/CartridgeTypes.h:294

Number of chip packets.

---

### switchPos

```cpp
isize switchPos
```

Defined in Media/Cartridges/CartridgeTypes.h:295

Position of the switch.

---

### led

```cpp
bool led
```

Defined in Media/Cartridges/CartridgeTypes.h:296

The LED is on.

