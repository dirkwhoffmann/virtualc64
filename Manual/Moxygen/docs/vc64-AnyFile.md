---
title: "AnyFile"
---
# AnyFile

```cpp
#include <Media/AnyFile.h>
```

```cpp
class AnyFile
```

Defined in Media/AnyFile.h:65

Base class of all media files

Provides basic functionality for reading and writing files, streams, and buffers.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::filesystem::path` | [`path`](#path)  | Physical location of this file. |
| `Buffer< u8 >` | [`data`](#data)  | The raw data of this file. |

---

### path

```cpp
std::filesystem::path path = ""
```

Defined in Media/AnyFile.h:70

Physical location of this file.

---

### data

```cpp
Buffer< u8 > data
```

Defined in Media/AnyFile.h:73

The raw data of this file.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| [`FileType`](vc64.md#filetype) | [`type`](#type) `virtual` `const` `inline` | Returns the media type of this file. |
|  | [`~AnyFile`](#anyfile-1) `virtual` `inline` | Destructor. |
| `void` | [`init`](#init)  | Initializes the file with an empty buffer of the given capacity. |
| `void` | [`init`](#init-1)  | Initializes the file with the contents of a buffer. |
| `void` | [`init`](#init-2)  | Initializes the file with the contents of a string. |
| `void` | [`init`](#init-3)  | Initializes the file with the contents of a file. |
| `void` | [`init`](#init-4)  | Initializes the file with the contents of a memory region. |
| `isize` | [`getSize`](#getsize) `virtual` `const` `inline` | Returns the size of this file. |
| `u8 *` | [`getData`](#getdata) `virtual` `const` `inline` | Returns a pointer to the file data. |
| `u8` | [`getData`](#getdata-1) `inline` | Returns a data byte. |
| `u64` | [`fnv64`](#fnv64) `virtual` `const` `inline` | Returns a FNV-64 fingerprint of this file. |
| `u32` | [`crc32`](#crc32) `virtual` `const` `inline` | Returns a CRC-32 fingerprint of this file. |
| `string` | [`name`](#name) `virtual` `const` | Returns the logical name of this file. |
| `PETName< 16 >` | [`getName`](#getname) `virtual` `const` | Returns the logical name of this file in PETSCII encoding. |
| `void` | [`strip`](#strip)  | Removes a certain number of bytes from the beginning of the file. |
| `void` | [`flash`](#flash) `virtual` `const` | Copies a portion of the file contents into a buffer. |
| `void` | [`flash`](#flash-1) `virtual` `const` | Copies the file contents (starting at an offset) into a buffer. |
| `isize` | [`writeToStream`](#writetostream)  | Writes a portion of the file to a stream. |
| `isize` | [`writeToStream`](#writetostream-1)  | Writes the file to a stream. |
| `isize` | [`writeToFile`](#writetofile)  | Writes a portion of the file to a file on disk. |
| `isize` | [`writeToFile`](#writetofile-1)  | Writes the file to a file on disk. |
| `isize` | [`writeToBuffer`](#writetobuffer)  | Writes a portion of the file to a memory buffer. |
| `isize` | [`writeToBuffer`](#writetobuffer-1)  | Writes a portion of the file to a buffer. |
| `isize` | [`writeToBuffer`](#writetobuffer-2)  | Writes the file to a memory buffer. |
| `isize` | [`writeToBuffer`](#writetobuffer-3)  | Writes the file to a buffer. |

---

### type

`virtual` `const` `inline`

```cpp
virtual inline FileType type() const
```

Defined in Media/AnyFile.h:123

Returns the media type of this file.

#### Reimplemented by

- [`Snapshot::type`](vc64-Snapshot.md#type-9)

---

### ~AnyFile

`virtual` `inline`

```cpp
virtual inline ~AnyFile()
```

Defined in Media/AnyFile.h:101

Destructor.

---

### init

```cpp
void init(isize capacity)
```

Defined in Media/AnyFile.h:104

Initializes the file with an empty buffer of the given capacity.

---

### init

```cpp
void init(const Buffer< u8 > & buffer)
```

Defined in Media/AnyFile.h:106

Initializes the file with the contents of a buffer.

---

### init

```cpp
void init(const string & str)
```

Defined in Media/AnyFile.h:108

Initializes the file with the contents of a string.

---

### init

```cpp
void init(const std::filesystem::path & path)
```

Defined in Media/AnyFile.h:110

Initializes the file with the contents of a file.

---

### init

```cpp
void init(const u8 * buf, isize len)
```

Defined in Media/AnyFile.h:112

Initializes the file with the contents of a memory region.

---

### getSize

`virtual` `const` `inline`

```cpp
virtual inline isize getSize() const
```

Defined in Media/AnyFile.h:127

Returns the size of this file.

---

### getData

`virtual` `const` `inline`

```cpp
virtual inline u8 * getData() const
```

Defined in Media/AnyFile.h:134

Returns a pointer to the file data.

---

### getData

`inline`

```cpp
inline u8 getData(isize nr)
```

Defined in Media/AnyFile.h:160

Returns a data byte.

---

### fnv64

`virtual` `const` `inline`

```cpp
virtual inline u64 fnv64() const
```

Defined in Media/AnyFile.h:141

Returns a FNV-64 fingerprint of this file.

---

### crc32

`virtual` `const` `inline`

```cpp
virtual inline u32 crc32() const
```

Defined in Media/AnyFile.h:143

Returns a CRC-32 fingerprint of this file.

---

### name

`virtual` `const`

```cpp
virtual string name() const
```

Defined in Media/AnyFile.h:154

Returns the logical name of this file.

---

### getName

`virtual` `const`

```cpp
virtual PETName< 16 > getName() const
```

Defined in Media/AnyFile.h:156

Returns the logical name of this file in PETSCII encoding.

---

### strip

```cpp
void strip(isize count)
```

Defined in Media/AnyFile.h:163

Removes a certain number of bytes from the beginning of the file.

---

### flash

`virtual` `const`

```cpp
virtual void flash(u8 * buf, isize offset, isize len) const
```

Defined in Media/AnyFile.h:172

Copies a portion of the file contents into a buffer.

---

### flash

`virtual` `const`

```cpp
virtual void flash(u8 * buf, isize offset = 0) const
```

Defined in Media/AnyFile.h:174

Copies the file contents (starting at an offset) into a buffer.

---

### writeToStream

```cpp
isize writeToStream(std::ostream & stream, isize offset, isize len)
```

Defined in Media/AnyFile.h:193

Writes a portion of the file to a stream.

---

### writeToStream

```cpp
isize writeToStream(std::ostream & stream)
```

Defined in Media/AnyFile.h:202

Writes the file to a stream.

---

### writeToFile

```cpp
isize writeToFile(const std::filesystem::path & path, isize offset, isize len)
```

Defined in Media/AnyFile.h:195

Writes a portion of the file to a file on disk.

---

### writeToFile

```cpp
isize writeToFile(const std::filesystem::path & path)
```

Defined in Media/AnyFile.h:204

Writes the file to a file on disk.

---

### writeToBuffer

```cpp
isize writeToBuffer(u8 * buf, isize offset, isize len)
```

Defined in Media/AnyFile.h:197

Writes a portion of the file to a memory buffer.

---

### writeToBuffer

```cpp
isize writeToBuffer(Buffer< u8 > & buffer, isize offset, isize len)
```

Defined in Media/AnyFile.h:199

Writes a portion of the file to a buffer.

---

### writeToBuffer

```cpp
isize writeToBuffer(u8 * buf)
```

Defined in Media/AnyFile.h:206

Writes the file to a memory buffer.

---

### writeToBuffer

```cpp
isize writeToBuffer(Buffer< u8 > & buffer)
```

Defined in Media/AnyFile.h:208

Writes the file to a buffer.

## Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| [`FileType`](vc64.md#filetype) | [`type`](#type-1) `static` | Determines the type of an arbitrary file on disk. |
| [`AnyFile`](#anyfile) * | [`make`](#make) `static` | Creates a media file from a file on disk. The file type is determined automatically. |
| [`AnyFile`](#anyfile) * | [`make`](#make-1) `static` | Creates a media file of a given type from a file on disk. |
| [`AnyFile`](#anyfile) * | [`make`](#make-2) `static` | Creates a media file of a given type from a memory buffer. |
| [`AnyFile`](#anyfile) * | [`make`](#make-3) `static` | Creates a media file of a given type from the contents of a drive. |

---

### type

`static`

```cpp
static FileType type(const fs::path & path)
```

Defined in Media/AnyFile.h:81

Determines the type of an arbitrary file on disk.

---

### make

`static`

```cpp
static AnyFile * make(const fs::path & path)
```

Defined in Media/AnyFile.h:85

Creates a media file from a file on disk. The file type is determined automatically.

---

### make

`static`

```cpp
static AnyFile * make(const fs::path & path, FileType type)
```

Defined in Media/AnyFile.h:87

Creates a media file of a given type from a file on disk.

---

### make

`static`

```cpp
static AnyFile * make(const u8 * buf, isize len, FileType type)
```

Defined in Media/AnyFile.h:89

Creates a media file of a given type from a memory buffer.

---

### make

`static`

```cpp
static AnyFile * make(class DriveAPI & drive, FileType type)
```

Defined in Media/AnyFile.h:91

Creates a media file of a given type from the contents of a drive.

