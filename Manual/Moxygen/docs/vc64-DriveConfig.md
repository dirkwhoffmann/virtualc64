---
title: "DriveConfig"
kind: struct
---
# DriveConfig

```cpp
#include <Peripherals/Drive/DriveTypes.h>
```

```cpp
struct DriveConfig
```

Defined in Peripherals/Drive/DriveTypes.h:192

Configuration of a drive.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`autoConfig`](#autoconfig)  | General. |
| [`DriveType`](enum-DriveType.md) | [`type`](#type-6)  | Drive model. |
| [`DriveRam`](enum-DriveRam.md) | [`ram`](#ram)  | Drive RAM. |
| [`ParCableType`](enum-ParCableType.md) | [`parCable`](#parcable)  | Parallel cable. |
| `bool` | [`powerSave`](#powersave)  | Take fast paths. |
| `bool` | [`connected`](#connected-1)  | The drive is connected to the serial port. |
| `bool` | [`switchedOn`](#switchedon)  | Power switch. |
| `isize` | [`ejectDelay`](#ejectdelay)  | Disk ejection delay. |
| `isize` | [`swapDelay`](#swapdelay)  | Disk swap delay. |
| `isize` | [`insertDelay`](#insertdelay)  | Disk insertion delay. |
| `i16` | [`pan`](#pan-1)  | Stereo pan of the drive sounds. |
| `u8` | [`powerVolume`](#powervolume)  | Power switch volume. |
| `u8` | [`stepVolume`](#stepvolume)  | [Head](vc64.md#head) step volume. |
| `u8` | [`insertVolume`](#insertvolume)  | Disk insertion volume. |
| `u8` | [`ejectVolume`](#ejectvolume)  | Disk ejection volume. |
| `bool` | [`saveRoms`](#saveroms)  | Save Roms. |

---

### autoConfig

```cpp
bool autoConfig
```

Defined in Peripherals/Drive/DriveTypes.h:195

General.

---

### type

```cpp
DriveType type
```

Type: [`DriveType`](enum-DriveType.md)

Defined in Peripherals/Drive/DriveTypes.h:198

Drive model.

---

### ram

```cpp
DriveRam ram
```

Type: [`DriveRam`](enum-DriveRam.md)

Defined in Peripherals/Drive/DriveTypes.h:199

Drive RAM.

---

### parCable

```cpp
ParCableType parCable
```

Type: [`ParCableType`](enum-ParCableType.md)

Defined in Peripherals/Drive/DriveTypes.h:200

Parallel cable.

---

### powerSave

```cpp
bool powerSave
```

Defined in Peripherals/Drive/DriveTypes.h:201

Take fast paths.

---

### connected

```cpp
bool connected
```

Defined in Peripherals/Drive/DriveTypes.h:204

The drive is connected to the serial port.

---

### switchedOn

```cpp
bool switchedOn
```

Defined in Peripherals/Drive/DriveTypes.h:205

Power switch.

---

### ejectDelay

```cpp
isize ejectDelay
```

Defined in Peripherals/Drive/DriveTypes.h:208

Disk ejection delay.

---

### swapDelay

```cpp
isize swapDelay
```

Defined in Peripherals/Drive/DriveTypes.h:209

Disk swap delay.

---

### insertDelay

```cpp
isize insertDelay
```

Defined in Peripherals/Drive/DriveTypes.h:210

Disk insertion delay.

---

### pan

```cpp
i16 pan
```

Defined in Peripherals/Drive/DriveTypes.h:213

Stereo pan of the drive sounds.

---

### powerVolume

```cpp
u8 powerVolume
```

Defined in Peripherals/Drive/DriveTypes.h:214

Power switch volume.

---

### stepVolume

```cpp
u8 stepVolume
```

Defined in Peripherals/Drive/DriveTypes.h:215

[Head](vc64.md#head) step volume.

---

### insertVolume

```cpp
u8 insertVolume
```

Defined in Peripherals/Drive/DriveTypes.h:216

Disk insertion volume.

---

### ejectVolume

```cpp
u8 ejectVolume
```

Defined in Peripherals/Drive/DriveTypes.h:217

Disk ejection volume.

---

### saveRoms

```cpp
bool saveRoms
```

Defined in Peripherals/Drive/DriveTypes.h:220

Save Roms.

