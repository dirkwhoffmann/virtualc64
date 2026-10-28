---
title: "DatasetteAPI"
---
# DatasetteAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class DatasetteAPI
```

Defined in VirtualC64.h:1057

Datasette Public [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`DatasetteInfo`](vc64-DatasetteInfo.md#datasetteinfo) & | [`getInfo`](#getinfo-4) `const` | Returns the component's current state. |
| const [`DatasetteInfo`](vc64-DatasetteInfo.md#datasetteinfo) & | [`getCachedInfo`](#getcachedinfo-4) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-4)). |
| `void` | [`insertTape`](#inserttape)  | Inserts a tape. |
| `void` | [`insertTapeAsync`](#inserttapeasync)  | Asynchronous version of [insertTape()](#inserttape): queues the request and returns at once. |
| `void` | [`ejectTape`](#ejecttape)  | Ejects a tape. This function has no effect if no tape is inserted. |
| `void` | [`exportTape`](#exporttape) `const` | Export tape to a file. |

---

### getInfo

`const`

```cpp
const DatasetteInfo & getInfo() const
```

Defined in VirtualC64.h:1074

Returns the component's current state.

[getInfo()](#getinfo-4) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-4) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const DatasetteInfo & getCachedInfo() const
```

Defined in VirtualC64.h:1076

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-4)).

---

### insertTape

```cpp
void insertTape(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1083

Inserts a tape.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | The tape to insert. |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be read or does not contain valid data. |

---

### insertTapeAsync

```cpp
void insertTapeAsync(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1090

Asynchronous version of [insertTape()](#inserttape): queues the request and returns at once.

The file is read on the emulator thread. Success and failure are both reported by the usual message (e.g., CRT_ATTACHED). If the request failed, the message carries the exception.

---

### ejectTape

```cpp
void ejectTape()
```

Defined in VirtualC64.h:1095

Ejects a tape. This function has no effect if no tape is inserted.

---

### exportTape

`const`

```cpp
void exportTape(const std::filesystem::path & path) const
```

Defined in VirtualC64.h:1100

Export tape to a file.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | Export path. |

