---
title: "Drives"
---
# Drives

The floppy drives and the disks in them.

## Classes

| Name | Description |
|------|-------------|
| [`DiskAPI`](vc64-DiskAPI.md#diskapi) | Disk Public [API](vc64-API.md#api) |
| [`DriveAPI`](vc64-DriveAPI.md#driveapi) | Drive Public [API](vc64-API.md#api) |
| [`DiskLayout`](vc64-DiskLayout.md#disklayout) | Layout of a disk. |
| [`DriveConfig`](vc64-DriveConfig.md#driveconfig) | Configuration of a drive. |
| [`DriveInfo`](vc64-DriveInfo.md#driveinfo) | State of a drive. |
| [`DriveMsg`](vc64-DriveMsg.md#drivemsg) | Payload of drive messages. |
| [`SectorLayout`](vc64-SectorLayout.md#sectorlayout) | Position of a sector on a track (in bits). |
| [`TrackDefaults`](vc64-TrackDefaults.md#trackdefaults) | Default properties of a track. |
| [`TrackLayout`](vc64-TrackLayout.md#tracklayout-1) | Layout of a track. |

## Enumerations

| Name | Description |
|------|-------------|
| [`DiskType`](#disktype)  | Disk type. |
| [`DriveRam`](#driveram)  | Memory expansion of a drive. |
| [`DriveType`](#drivetype)  | Drive model. |
| [`DrvMemType`](#drvmemtype)  | Memory types of a drive. |
| [`InsertionStatus`](#insertionstatus)  | Insertion state of a disk. |
| [`ParCableType`](#parcabletype)  | Parallel cable type. |

---

### DiskType

```cpp
enum DiskType
```

Defined in Peripherals/Drive/FloppyDiskTypes.h:110

Disk type.

| Value | Description |
|-------|-------------|
| `SS_SD` | Single-sided, single density (VC1541). |
| `DS_SD` | Double-sided, single density (VC1571) (unsupported). |

---

### DriveRam

```cpp
enum DriveRam
```

Defined in Peripherals/Drive/DriveTypes.h:85

Memory expansion of a drive.

| Value | Description |
|-------|-------------|
| `NONE` |  |
| `RANGE_8000_9FFF` | 8K |
| `RANGE_6000_7FFF` | 8K |
| `RANGE_4000_7FFF` | 16K |

---

### DriveType

```cpp
enum DriveType
```

Defined in Peripherals/Drive/DriveTypes.h:55

Drive model.

| Value | Description |
|-------|-------------|
| `VC1541` |  |
| `VC1541C` |  |
| `VC1541II` |  |

---

### DrvMemType

```cpp
enum DrvMemType
```

Defined in Peripherals/Drive/DriveTypes.h:117

Memory types of a drive.

| Value | Description |
|-------|-------------|
| `NONE` | Unmapped. |
| `RAM` |  |
| `EXP` | Expansion RAM. |
| `ROM` |  |
| `VIA1` |  |
| `VIA2` |  |
| `PIA` | Peripheral Interface Adapter (Dolphin3). |

---

### InsertionStatus

```cpp
enum InsertionStatus
```

Defined in Peripherals/Drive/DriveTypes.h:155

Insertion state of a disk.

| Value | Description |
|-------|-------------|
| `FULLY_EJECTED` |  |
| `PARTIALLY_INSERTED` |  |
| `FULLY_INSERTED` |  |
| `PARTIALLY_EJECTED` |  |

---

### ParCableType

```cpp
enum ParCableType
```

Defined in Peripherals/Drive/ParallelCableTypes.h:25

Parallel cable type.

| Value | Description |
|-------|-------------|
| `NONE` | No parallel cable attached. |
| `STANDARD` | Standard parallel cable. |
| `DOLPHIN3` | DolphinDOS cable. |
