---
title: "DriveAPI"
---
# DriveAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class DriveAPI
```

Defined in VirtualC64.h:1316

Drive Public [API](vc64-API.md#api)

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`DiskAPI`](vc64-DiskAPI.md#diskapi) | [`disk`](#disk)  | The disk in the drive. |

---

### disk

```cpp
DiskAPI disk
```

Type: [`DiskAPI`](vc64-DiskAPI.md#diskapi)

Defined in VirtualC64.h:1325

The disk in the drive.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`DriveConfig`](vc64-DriveConfig.md#driveconfig) & | [`getConfig`](#getconfig-5) `const` | Returns the component's current configuration. |
| const [`DriveInfo`](vc64-DriveInfo.md#driveinfo) & | [`getInfo`](#getinfo-5) `const` | Returns the component's current state. |
| const [`DriveInfo`](vc64-DriveInfo.md#driveinfo) & | [`getCachedInfo`](#getcachedinfo-5) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-5)). |
| `void` | [`insertBlankDisk`](#insertblankdisk)  | Inserts a new disk. |
| `void` | [`insert`](#insert)  | Inserts a disk created from an image file. |
| `void` | [`insertAsync`](#insertasync)  | Asynchronous version of [insert()](#insert): queues the request and returns at once. |
| `void` | [`ejectDisk`](#ejectdisk)  | Ejects the current disk. |
| `void` | [`save`](#save)  | Saves the current disk as an image file. |
| `void` | [`saveFiles`](#savefiles)  | Exports the current disk to a file. |

---

### getConfig

`const`

```cpp
const DriveConfig & getConfig() const
```

Defined in VirtualC64.h:1329

Returns the component's current configuration.

---

### getInfo

`const`

```cpp
const DriveInfo & getInfo() const
```

Defined in VirtualC64.h:1340

Returns the component's current state.

[getInfo()](#getinfo-5) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-5) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const DriveInfo & getCachedInfo() const
```

Defined in VirtualC64.h:1342

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-5)).

---

### insertBlankDisk

```cpp
void insertBlankDisk(FSFormat fstype, string name)
```

Defined in VirtualC64.h:1348

Inserts a new disk.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `fstype` | `FSFormat` | The file system the disk should be formatted with. |
| `name` | `string` | A PET string with the name of the new disk. |

---

### insert

```cpp
void insert(const std::filesystem::path & path, bool wp)
```

Defined in VirtualC64.h:1356

Inserts a disk created from an image file.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Path to the disk image. |
| `wp` | `bool` | Write-protection status of the disk. |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be read or does not contain valid data. |

---

### insertAsync

```cpp
void insertAsync(const std::filesystem::path & path, bool wp)
```

Defined in VirtualC64.h:1363

Asynchronous version of [insert()](#insert): queues the request and returns at once.

The file is read on the emulator thread. Success and failure are both reported by the usual message (e.g., CRT_ATTACHED). If the request failed, the message carries the exception.

---

### ejectDisk

```cpp
void ejectDisk()
```

Defined in VirtualC64.h:1367

Ejects the current disk.

---

### save

```cpp
void save(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1372

Saves the current disk as an image file.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Path to the destination file. |

---

### saveFiles

```cpp
void saveFiles(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1377

Exports the current disk to a file.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Path to the destination folder. |

