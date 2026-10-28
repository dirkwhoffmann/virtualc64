---
title: "Snapshot"
---
# Snapshot

```cpp
#include <Media/Snapshot.h>
```

```cpp
class Snapshot
```

Defined in Media/Snapshot.h:69

A snapshot of the emulator state

A snapshot contains the complete internal state of the emulator together with a preview image. Snapshots are created with [C64API::takeSnapshot()](vc64-C64API.md#takesnapshot) and can be restored with [C64API::loadSnapshot()](vc64-C64API.md#loadsnapshot).

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Snapshot`](#snapshot-1) `inline` | Creates a copy of a snapshot. |
|  | [`Snapshot`](#snapshot-2) `inline` | Reads a snapshot from a file. |
|  | [`Snapshot`](#snapshot-3) `inline` | Reads a snapshot from a memory buffer. |
|  | [`Snapshot`](#snapshot-4)  | Creates an empty snapshot with the given capacity. |
|  | [`Snapshot`](#snapshot-5)  | Takes a snapshot of the emulator state. |
|  | [`Snapshot`](#snapshot-6)  | Takes a snapshot of the emulator state and compresses it. |
| [`FileType`](enum-FileType.md) | [`type`](#type-9) `virtual` `const` `inline` `override` | Returns the media type of this file. |
| `std::pair< isize, isize >` | [`previewImageSize`](#previewimagesize) `const` | Returns the size of the preview image (width, height). |
| `const u32 *` | [`previewImageData`](#previewimagedata) `const` | Returns the pixel data of the preview image. |
| `time_t` | [`timestamp`](#timestamp) `const` | Returns the time the snapshot was taken. |
| `bool` | [`isTooOld`](#istooold) `const` | Returns true if the snapshot was created by an older, incompatible version. |
| `bool` | [`isTooNew`](#istoonew) `const` | Returns true if the snapshot was created by a newer version. |
| `bool` | [`isBeta`](#isbeta) `const` | Returns true if the snapshot was created by a beta version. |
| `bool` | [`matches`](#matches) `inline` | Returns true if the snapshot can be restored by this version. |
| [`SnapshotHeader`](vc64-SnapshotHeader.md#snapshotheader) * | [`getHeader`](#getheader) `const` `inline` | Returns a pointer to the snapshot header. |
| const [`Thumbnail`](vc64-Thumbnail.md#thumbnail) & | [`getThumbnail`](#getthumbnail) `const` `inline` | Returns the preview image. |
| `u8 *` | [`getSnapshotData`](#getsnapshotdata) `const` `inline` | Returns a pointer to the core data. |
| `void` | [`takeScreenshot`](#takescreenshot)  | Records a screenshot. |
| `utl::Compressor` | [`compressor`](#compressor) `const` `inline` | Returns the compression method. |
| `bool` | [`isCompressed`](#iscompressed) `const` `inline` | Returns true if the snapshot is compressed. |
| `void` | [`compress`](#compress)  | Compresses the snapshot with the given method. |
| `void` | [`uncompress`](#uncompress)  | Uncompresses the snapshot. |

---

### Snapshot

`inline`

```cpp
inline Snapshot(const Snapshot & other)
```

Defined in Media/Snapshot.h:88

Creates a copy of a snapshot.

---

### Snapshot

`inline`

```cpp
inline Snapshot(const fs::path & path)
```

Defined in Media/Snapshot.h:90

Reads a snapshot from a file.

---

### Snapshot

`inline`

```cpp
inline Snapshot(const u8 * buf, isize len)
```

Defined in Media/Snapshot.h:92

Reads a snapshot from a memory buffer.

---

### Snapshot

```cpp
Snapshot(isize capacity)
```

Defined in Media/Snapshot.h:94

Creates an empty snapshot with the given capacity.

---

### Snapshot

```cpp
Snapshot(C64 & c64)
```

Defined in Media/Snapshot.h:96

Takes a snapshot of the emulator state.

---

### Snapshot

```cpp
Snapshot(C64 & c64, utl::Compressor compressor)
```

Defined in Media/Snapshot.h:98

Takes a snapshot of the emulator state and compresses it.

---

### type

`virtual` `const` `inline` `override`

```cpp
virtual inline FileType type() const override
```

Defined in Media/Snapshot.h:112

Returns the media type of this file.

#### Reimplements

- [`AnyFile::type`](vc64-AnyFile.md#type)

---

### previewImageSize

`const`

```cpp
std::pair< isize, isize > previewImageSize() const
```

Defined in Media/Snapshot.h:123

Returns the size of the preview image (width, height).

---

### previewImageData

`const`

```cpp
const u32 * previewImageData() const
```

Defined in Media/Snapshot.h:125

Returns the pixel data of the preview image.

---

### timestamp

`const`

```cpp
time_t timestamp() const
```

Defined in Media/Snapshot.h:127

Returns the time the snapshot was taken.

---

### isTooOld

`const`

```cpp
bool isTooOld() const
```

Defined in Media/Snapshot.h:131

Returns true if the snapshot was created by an older, incompatible version.

---

### isTooNew

`const`

```cpp
bool isTooNew() const
```

Defined in Media/Snapshot.h:133

Returns true if the snapshot was created by a newer version.

---

### isBeta

`const`

```cpp
bool isBeta() const
```

Defined in Media/Snapshot.h:135

Returns true if the snapshot was created by a beta version.

---

### matches

`inline`

```cpp
inline bool matches()
```

Defined in Media/Snapshot.h:137

Returns true if the snapshot can be restored by this version.

---

### getHeader

`const` `inline`

```cpp
inline SnapshotHeader * getHeader() const
```

Defined in Media/Snapshot.h:141

Returns a pointer to the snapshot header.

---

### getThumbnail

`const` `inline`

```cpp
inline const Thumbnail & getThumbnail() const
```

Defined in Media/Snapshot.h:145

Returns the preview image.

---

### getSnapshotData

`const` `inline`

```cpp
inline u8 * getSnapshotData() const
```

Defined in Media/Snapshot.h:149

Returns a pointer to the core data.

---

### takeScreenshot

```cpp
void takeScreenshot(C64 & c64)
```

Defined in Media/Snapshot.h:152

Records a screenshot.

---

### compressor

`const` `inline`

```cpp
inline utl::Compressor compressor() const
```

Defined in Media/Snapshot.h:161

Returns the compression method.

---

### isCompressed

`const` `inline`

```cpp
inline bool isCompressed() const
```

Defined in Media/Snapshot.h:165

Returns true if the snapshot is compressed.

---

### compress

```cpp
void compress(utl::Compressor method)
```

Defined in Media/Snapshot.h:169

Compresses the snapshot with the given method.

---

### uncompress

```cpp
void uncompress()
```

Defined in Media/Snapshot.h:171

Uncompresses the snapshot.

## Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`isCompatible`](#iscompatible) `static` | Checks whether a file, a buffer or a memory region contains a snapshot. |

---

### isCompatible

`static`

```cpp
static bool isCompatible(const fs::path & path)
```

Defined in Media/Snapshot.h:78

Checks whether a file, a buffer or a memory region contains a snapshot.

