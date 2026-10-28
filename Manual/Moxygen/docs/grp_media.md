---
title: "Media files"
---
# Media files

Snapshots, media files, and their types.

## Classes

| Name | Description |
|------|-------------|
| [`AnyFile`](vc64-AnyFile.md#anyfile) | Base class of all media files |
| [`Snapshot`](vc64-Snapshot.md#snapshot) | A snapshot of the emulator state |
| [`SnapshotHeader`](vc64-SnapshotHeader.md#snapshotheader) | Header of a snapshot. |
| [`Thumbnail`](vc64-Thumbnail.md#thumbnail) | Preview image of a snapshot. |

## Enumerations

| Name | Description |
|------|-------------|
| [`CBMFileType`](#cbmfiletype)  |  |
| [`FileError`](#fileerror)  | Error codes for file operations. |
| [`FileType`](#filetype)  | Supported media file types. |
| [`TAPVersion`](#tapversion)  | Version of the TAP file format. |

---

### CBMFileType

```cpp
enum CBMFileType
```

Defined in Peripherals/Drive/FloppyDiskTypes.h:137

| Value | Description |
|-------|-------------|
| `PRG` |  |
| `SEQ` |  |
| `USR` |  |
| `REL` |  |

---

### FileError

```cpp
enum FileError
```

Defined in Media/MediaFileTypes.h:105

Error codes for file operations.

| Value | Description |
|-------|-------------|
| `FILE_OK` |  |
| `FILE_NOT_FOUND` |  |
| `INVALID_TYPE` |  |
| `CANT_READ` |  |
| `CANT_WRITE` |  |
| `OUT_OF_MEMORY` |  |
| `UNKNOWN` |  |
| `UNSUPPORTED_SNAPSHOT` |  |

---

### FileType

```cpp
enum FileType
```

Defined in Media/MediaFileTypes.h:25

Supported media file types.

| Value | Description |
|-------|-------------|
| `UNKNOWN` |  |
| `WORKSPACE` |  |
| `SNAPSHOT` |  |
| `SCRIPT` |  |
| `CRT` |  |
| `T64` |  |
| `P00` |  |
| `PRG` |  |
| `G64` |  |
| `TAP` |  |
| `BASIC_ROM` |  |
| `CHAR_ROM` |  |
| `KERNAL_ROM` |  |
| `VC1541_ROM` |  |

---

### TAPVersion

```cpp
enum TAPVersion
```

Defined in Media/MediaFileTypes.h:77

Version of the TAP file format.

| Value | Description |
|-------|-------------|
| `ORIGINAL` |  |
| `ADVANCED` |  |
