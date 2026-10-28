---
title: "C64API"
---
# C64API

```cpp
#include <VirtualC64.h>
```

```cpp
class C64API
```

Defined in VirtualC64.h:1739

C64 [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`C64Config`](vc64-C64Config.md#c64config) & | [`getConfig`](#getconfig-1) `const` | Returns the component's current configuration. |
| const [`C64Info`](vc64-C64Info.md#c64info) & | [`getInfo`](#getinfo-1) `const` | Returns the component's current state. |
| const [`C64Info`](vc64-C64Info.md#c64info) & | [`getCachedInfo`](#getcachedinfo-1) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-1)). |
| `void` | [`dump`](#dump) `const` | Prints debug information about the component. |
| `RomTraits` | [`getRomTraits`](#getromtraits) `const` | Returns information about one of the installed Roms. |
| `bool` | [`getMsg`](#getmsg)  | Reads a message from the message queue. |
| `u64` | [`getAutoInspectionMask`](#getautoinspectionmask) `const` | Gets the current auto-inspection mask The GUI utilizes Auto-Inspection to display life updates of the internal emulator state in the Inspector panel. As soon as an auto-inspection mask is set, the emulator caches the internal states of the inspected components at periodic intervals. The inspected components are specified as a bit mask. |
| `void` | [`setAutoInspectionMask`](#setautoinspectionmask)  | Sets the current auto-inspection mask. |
| `void` | [`setAutoInspectionMaskAsync`](#setautoinspectionmaskasync)  | Asynchronous version of [setAutoInspectionMask()](#setautoinspectionmask): queues the request and returns at once. |
| `void` | [`loadWorkspace`](#loadworkspace)  | Loads a workspace. |
| `void` | [`loadWorkspace`](#loadworkspace-1)  | Loads, saves, or asynchronously saves the workspace of an SVM. |
| `void` | [`saveWorkspace`](#saveworkspace) `const` | Saves a workspace to disk. |
| `void` | [`saveWorkspace`](#saveworkspace-1) `const` | Saves the workspace into an SVM that is already open (see [loadWorkspace()](#loadworkspace)). |
| `void` | [`saveWorkspaceAsync`](#saveworkspaceasync) `const` | Saves a workspace to disk asynchronously. |
| `void` | [`saveWorkspaceAsync`](#saveworkspaceasync-1) `const` | Asynchronous version of [saveWorkspace()](#saveworkspace): queues the request and returns at once. |
| std::unique_ptr< [`Snapshot`](vc64-Snapshot.md#snapshot) > | [`takeSnapshot`](#takesnapshot)  | Takes a snapshot. |
| `void` | [`loadSnapshot`](#loadsnapshot)  | Loads a snapshot. |
| `void` | [`loadSnapshot`](#loadsnapshot-1)  | Loads a snapshot. |
| `void` | [`loadSnapshotAsync`](#loadsnapshotasync)  | Asynchronous version of [loadSnapshot()](#loadsnapshot): queues the request and returns at once. |
| `void` | [`saveSnapshot`](#savesnapshot) `const` | Saves a snapshot to disk. |
| `void` | [`saveSnapshot`](#savesnapshot-1) `const` | Saves a snapshot, including a screenshot, into an SVM. |
| `void` | [`saveSnapshotAsync`](#savesnapshotasync) `const` | Saves a snapshot to disk asynchronously. |
| `void` | [`saveSnapshotAsync`](#savesnapshotasync-1) `const` | Asynchronous version of [saveSnapshot()](#savesnapshot): queues the request and returns at once. |
| `void` | [`hibernate`](#hibernate) `const` | Hibernates the machine into a virtual machine bundle. |
| `void` | [`hibernate`](#hibernate-1) `const` | Hibernates the machine into an SVM that is already open. |
| `void` | [`hibernateAsync`](#hibernateasync) `const` | Hibernates the machine asynchronously. |
| `void` | [`hibernateAsync`](#hibernateasync-1) `const` | Asynchronous version of [hibernate()](#hibernate): queues the request and returns at once. |
| `void` | [`loadRom`](#loadrom)  | Loads a ROM from a file The ROM type is determined automatically. |
| `void` | [`loadRom`](#loadrom-1)  | Loads a ROM from a file and installs it as the given ROM type. |
| `void` | [`loadRomAsync`](#loadromasync)  | Asynchronous version of [loadRom()](#loadrom): queues the request and returns at once. |
| `void` | [`loadRomAsync`](#loadromasync-1)  | Asynchronous version of loadRom(path, rom). |
| `void` | [`deleteRom`](#deleterom)  | Removes an installed ROM The ROM contents is overwritten with zeroes. |
| `void` | [`deleteRoms`](#deleteroms)  | Removes all installed ROMs including the floppy drive ROMs. The ROM contents is overwritten with zeroes. |
| `void` | [`saveRom`](#saverom)  | Saves a ROM to disk. |
| `void` | [`installOpenRom`](#installopenrom)  | Installes a MEGA65 OpenRom. |
| `void` | [`installOpenRoms`](#installopenroms)  | Install all three MEGA65 OpenROMs. |
| `void` | [`flash`](#flash-2)  | Flashes a file into memory. |
| `void` | [`flashAsync`](#flashasync)  | Asynchronous version of [flash()](#flash-2): queues the request and returns at once. |

---

### getConfig

`const`

```cpp
const C64Config & getConfig() const
```

Defined in VirtualC64.h:1752

Returns the component's current configuration.

---

### getInfo

`const`

```cpp
const C64Info & getInfo() const
```

Defined in VirtualC64.h:1763

Returns the component's current state.

[getInfo()](#getinfo-1) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-1) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const C64Info & getCachedInfo() const
```

Defined in VirtualC64.h:1765

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-1)).

---

### dump

`const`

```cpp
void dump(Category category, std::ostream & os) const
```

Defined in VirtualC64.h:1772

Prints debug information about the component.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `category` | `Category` | Debug information category |
| `os` | `std::ostream &` | Output stream |

---

### getRomTraits

`const`

```cpp
RomTraits getRomTraits(RomType type) const
```

Defined in VirtualC64.h:1778

Returns information about one of the installed Roms.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `RomType` | The ROM type |

---

### getMsg

```cpp
bool getMsg(Message & msg, std::exception_ptr & exception)
```

Defined in VirtualC64.h:1791

Reads a message from the message queue.

#### Returns
True if a message could be read, false if the queue was empty

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `msg` | [`Message`](vc64-Message.md#message) & | The returned message (if any) |
| `exception` | `std::exception_ptr &` | The exception the message reports (null if none) |

---

### getAutoInspectionMask

`const`

```cpp
u64 getAutoInspectionMask() const
```

Defined in VirtualC64.h:1807

Gets the current auto-inspection mask The GUI utilizes Auto-Inspection to display life updates of the internal emulator state in the Inspector panel. As soon as an auto-inspection mask is set, the emulator caches the internal states of the inspected components at periodic intervals. The inspected components are specified as a bit mask.

#### Returns
A bit mask indicating the components under inspection

---

### setAutoInspectionMask

```cpp
void setAutoInspectionMask(u64 mask)
```

Defined in VirtualC64.h:1822

Sets the current auto-inspection mask.

Each bit selects one component class (the bit position is the numeric value of the Class entry). A component's [getCachedInfo()](#getcachedinfo-1) and getCachedMetrics() only return up-to-date snapshots while its class is selected.

Example: The following call enables auto-inspections for the CIA chips and the CPU: setAutoInspectionMask(1LL << long(Class::CIA) | 1LL << long(Class::CPU));

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `mask` | `u64` | A bit mask indicating the components under inspection |

---

### setAutoInspectionMaskAsync

```cpp
void setAutoInspectionMaskAsync(u64 mask)
```

Defined in VirtualC64.h:1825

Asynchronous version of [setAutoInspectionMask()](#setautoinspectionmask): queues the request and returns at once.

---

### loadWorkspace

```cpp
void loadWorkspace(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1838

Loads a workspace.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Path to a workspace folder |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be read or does not contain valid data. |

---

### loadWorkspace

```cpp
void loadWorkspace(retro::vault::SVMFile & svm)
```

Defined in VirtualC64.h:1870

Loads, saves, or asynchronously saves the workspace of an SVM.

These work on the workspace folder of an SVM that is already open. Saving replaces the workspace, notes its preview image in the manifest and persists the SVM. As with hibernateAsync, the object is used on the emulator thread and must stay alive, and untouched, until the WORKSPACE_SAVED message has arrived.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the SVM file cannot be read or written, or if its content is invalid (synchronous versions). |

---

### saveWorkspace

`const`

```cpp
void saveWorkspace(const std::filesystem::path & path) const
```

Defined in VirtualC64.h:1846

Saves a workspace to disk.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Destination path |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be written. |

---

### saveWorkspace

`const`

```cpp
void saveWorkspace(retro::vault::SVMFile & svm) const
```

Defined in VirtualC64.h:1872

Saves the workspace into an SVM that is already open (see [loadWorkspace()](#loadworkspace)).

---

### saveWorkspaceAsync

`const`

```cpp
void saveWorkspaceAsync(const std::filesystem::path & path) const
```

Defined in VirtualC64.h:1857

Saves a workspace to disk asynchronously.

The request is handed to the emulator thread, which carries it out when it processes its command queue. The function returns immediately. The GUI is informed by a WORKSPACE_SAVED message, which carries an exception if the operation failed.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Destination path |

---

### saveWorkspaceAsync

`const`

```cpp
void saveWorkspaceAsync(retro::vault::SVMFile & svm) const
```

Defined in VirtualC64.h:1874

Asynchronous version of [saveWorkspace()](#saveworkspace): queues the request and returns at once.

---

### takeSnapshot

```cpp
std::unique_ptr< Snapshot > takeSnapshot(utl::Compressor compressor, isize delay = 0, bool repeat = false)
```

Defined in VirtualC64.h:1887

Takes a snapshot.

#### Returns
A pointer to the created [Snapshot](vc64-Snapshot.md#snapshot) object.

> [!NOTE]
> The function transfers the ownership to the caller. It is his responsibility of the caller to free the object.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `compressor` | `utl::Compressor` | The snapshot compression method |
| `delay` | `isize` | Optional delay in seconds |
| `repeat` | `bool` | if true, snapshots are taken continuously |

---

### loadSnapshot

```cpp
void loadSnapshot(const Snapshot & snapshot)
```

Defined in VirtualC64.h:1893

Loads a snapshot.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `snapshot` | const [`Snapshot`](vc64-Snapshot.md#snapshot) & | Reference to a snapshot |

---

### loadSnapshot

```cpp
void loadSnapshot(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1901

Loads a snapshot.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Source path |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be read or does not contain valid data. |

---

### loadSnapshotAsync

```cpp
void loadSnapshotAsync(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1907

Asynchronous version of [loadSnapshot()](#loadsnapshot): queues the request and returns at once.

Success and failure are both reported by the usual message (SNAPSHOT_RESTORED). If the request failed, the message carries the exception.

---

### saveSnapshot

`const`

```cpp
void saveSnapshot(const std::filesystem::path & path, utl::Compressor compressor) const
```

Defined in VirtualC64.h:1916

Saves a snapshot to disk.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Destination path |
| `compressor` | `utl::Compressor` | The compression method applied to the snapshot data. |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be written. |

---

### saveSnapshot

`const`

```cpp
void saveSnapshot(retro::vault::SVMFile & svm, utl::Compressor compressor) const
```

Defined in VirtualC64.h:1941

Saves a snapshot, including a screenshot, into an SVM.

Works on an SVM that is already open: the snapshot is added to its manifest, and the SVM is persisted. As with hibernateAsync, the object is used on the emulator thread by the asynchronous version and must stay alive, and untouched, until the SNAPSHOT_SAVED message has arrived.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the SVM file cannot be read or written, or if its content is invalid (synchronous versions). |

---

### saveSnapshotAsync

`const`

```cpp
void saveSnapshotAsync(const std::filesystem::path & path, utl::Compressor compressor) const
```

Defined in VirtualC64.h:1928

Saves a snapshot to disk asynchronously.

The request is handed to the emulator thread, which carries it out when it processes its command queue. The function returns immediately. The GUI is informed by a SNAPSHOT_SAVED message, which carries an exception if the operation failed.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Destination path |
| `compressor` | `utl::Compressor` | The compression method applied to the snapshot data. |

---

### saveSnapshotAsync

`const`

```cpp
void saveSnapshotAsync(retro::vault::SVMFile & svm, utl::Compressor compressor) const
```

Defined in VirtualC64.h:1943

Asynchronous version of [saveSnapshot()](#savesnapshot): queues the request and returns at once.

---

### hibernate

`const`

```cpp
void hibernate(const std::filesystem::path & path, bool snapshot, bool workspace) const
```

Defined in VirtualC64.h:1955

Hibernates the machine into a virtual machine bundle.

Saves the workspace (if workspace is true) and a snapshot including a screenshot (if snapshot is true) into the SVM, and updates its manifest.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Path of the SVM |
| `snapshot` | `bool` | Save a snapshot |
| `workspace` | `bool` | Save the workspace |

---

### hibernate

`const`

```cpp
void hibernate(retro::vault::SVMFile & svm, bool snapshot, bool workspace) const
```

Defined in VirtualC64.h:1976

Hibernates the machine into an SVM that is already open.

Works like the versions that take a path, but updates and persists the given object, so its manifest is current afterwards. The asynchronous version uses the object on the emulator thread: it must stay alive and must not be touched until the HIBERNATED message has arrived.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the SVM file cannot be read or written, or if its content is invalid (synchronous versions). |

---

### hibernateAsync

`const`

```cpp
void hibernateAsync(const std::filesystem::path & path, bool snapshot, bool workspace) const
```

Defined in VirtualC64.h:1964

Hibernates the machine asynchronously.

The request is handed to the emulator thread, which carries it out when it processes its command queue. The function returns immediately. The GUI is informed by a HIBERNATED message, which carries an error code and description if the operation failed.

---

### hibernateAsync

`const`

```cpp
void hibernateAsync(retro::vault::SVMFile & svm, bool snapshot, bool workspace) const
```

Defined in VirtualC64.h:1978

Asynchronous version of [hibernate()](#hibernate): queues the request and returns at once.

---

### loadRom

```cpp
void loadRom(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1990

Loads a ROM from a file The ROM type is determined automatically.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be read or does not contain valid data. |

---

### loadRom

```cpp
void loadRom(const std::filesystem::path & path, RomType rom)
```

Defined in VirtualC64.h:1992

Loads a ROM from a file and installs it as the given ROM type.

---

### loadRomAsync

```cpp
void loadRomAsync(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1998

Asynchronous version of [loadRom()](#loadrom): queues the request and returns at once.

Success and failure are both reported by a ROM_LOADED message. If the request failed, the message carries the exception.

---

### loadRomAsync

```cpp
void loadRomAsync(const std::filesystem::path & path, RomType rom)
```

Defined in VirtualC64.h:2000

Asynchronous version of loadRom(path, rom).

---

### deleteRom

```cpp
void deleteRom(RomType type)
```

Defined in VirtualC64.h:2005

Removes an installed ROM The ROM contents is overwritten with zeroes.

---

### deleteRoms

```cpp
void deleteRoms()
```

Defined in VirtualC64.h:2010

Removes all installed ROMs including the floppy drive ROMs. The ROM contents is overwritten with zeroes.

---

### saveRom

```cpp
void saveRom(const std::filesystem::path & path, RomType rom)
```

Defined in VirtualC64.h:2016

Saves a ROM to disk.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be written. |

---

### installOpenRom

```cpp
void installOpenRom(RomType type)
```

Defined in VirtualC64.h:2020

Installes a MEGA65 OpenRom.

---

### installOpenRoms

```cpp
void installOpenRoms()
```

Defined in VirtualC64.h:2024

Install all three MEGA65 OpenROMs.

---

### flash

```cpp
void flash(const std::filesystem::path & path, isize item = 0)
```

Defined in VirtualC64.h:2033

Flashes a file into memory.

---

### flashAsync

```cpp
void flashAsync(const std::filesystem::path & path, isize item = 0)
```

Defined in VirtualC64.h:2040

Asynchronous version of [flash()](#flash-2): queues the request and returns at once.

The file is read on the emulator thread. Success and failure are both reported by the usual message (e.g., CRT_ATTACHED). If the request failed, the message carries the exception.

