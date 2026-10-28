---
generator: doxide
---


# AnyFile

**`class AnyFile : public CoreObject`**

Base class of all media files

Provides basic functionality for reading and writing files, streams, and buffers.


## Variables

| Name | Description |
| ---- | ----------- |
| [path](#path) | Physical location of this file  |
| [data](#data) | The raw data of this file  |

## Functions

| Name | Description |
| ---- | ----------- |
| [type](#type) | Determines the type of an arbitrary file on disk  |
| [make](#make) | Creates a media file from a file on disk. |
| [make](#make) | Creates a media file of a given type from a file on disk  |
| [make](#make) | Creates a media file of a given type from a memory buffer  |
| [make](#make) | Creates a media file of a given type from the contents of a drive  |
| [~AnyFile](#_u007eAnyFile) | Destructor  |
| [init](#init) | Initializes the file with an empty buffer of the given capacity  |
| [init](#init) | Initializes the file with the contents of a buffer  |
| [init](#init) | Initializes the file with the contents of a string  |
| [init](#init) | Initializes the file with the contents of a file  |
| [init](#init) | Initializes the file with the contents of a memory region  |
| [type](#type) | Returns the media type of this file  |
| [getSize](#getSize) | Returns the size of this file  |
| [getData](#getData) | Returns a pointer to the file data  |
| [fnv64](#fnv64) | Returns a FNV-64 fingerprint of this file  |
| [crc32](#crc32) | Returns a CRC-32 fingerprint of this file  |
| [name](#name) | Returns the logical name of this file  |
| [getName](#getName) | Returns the logical name of this file in PETSCII encoding  |
| [getData](#getData) | Returns a data byte  |
| [strip](#strip) | Removes a certain number of bytes from the beginning of the file  |
| [flash](#flash) | Copies a portion of the file contents into a buffer  |
| [flash](#flash) | Copies the file contents (starting at an offset) into a buffer  |
| [writeToStream](#writeToStream) | Writes a portion of the file to a stream  |
| [writeToFile](#writeToFile) | Writes a portion of the file to a file on disk  |
| [writeToBuffer](#writeToBuffer) | Writes a portion of the file to a memory buffer  |
| [writeToBuffer](#writeToBuffer) | Writes a portion of the file to a buffer  |
| [writeToStream](#writeToStream) | Writes the file to a stream  |
| [writeToFile](#writeToFile) | Writes the file to a file on disk  |
| [writeToBuffer](#writeToBuffer) | Writes the file to a memory buffer  |
| [writeToBuffer](#writeToBuffer) | Writes the file to a buffer  |

## Variable Details

### data<a name="data"></a>

!!! variable "Buffer&lt;u8&gt; data"
    
    The raw data of this file
    

### path<a name="path"></a>

!!! variable "std::filesystem::path path"
    
    Physical location of this file
    

## Function Details

### crc32<a name="crc32"></a>
!!! function "virtual u32 crc32() const"
    
    Returns a CRC-32 fingerprint of this file
    

### flash<a name="flash"></a>
!!! function "virtual void flash(u8 &#42;buf, isize offset, isize len) const"
    
    Copies a portion of the file contents into a buffer
    

!!! function "virtual void flash(u8 &#42;buf, isize offset = 0) const"
    
    Copies the file contents (starting at an offset) into a buffer
    

### fnv64<a name="fnv64"></a>
!!! function "virtual u64 fnv64() const"
    
    Returns a FNV-64 fingerprint of this file
    

### getData<a name="getData"></a>
!!! function "virtual u8 &#42;getData() const"
    
    Returns a pointer to the file data
    

!!! function "u8 getData(isize nr)"
    
    Returns a data byte
    

### getName<a name="getName"></a>
!!! function "virtual PETName&lt;16&gt; getName() const"
    
    Returns the logical name of this file in PETSCII encoding
    

### getSize<a name="getSize"></a>
!!! function "virtual isize getSize() const"
    
    Returns the size of this file
    

### init<a name="init"></a>
!!! function "void init(isize capacity)"
    
    Initializes the file with an empty buffer of the given capacity
    

!!! function "void init(const Buffer&lt;u8&gt; &amp;buffer)"
    
    Initializes the file with the contents of a buffer
    

!!! function "void init(const string &amp;str)"
    
    Initializes the file with the contents of a string
    

!!! function "void init(const std::filesystem::path &amp;path)"
    
    Initializes the file with the contents of a file
    

!!! function "void init(const u8 &#42;buf, isize len)"
    
    Initializes the file with the contents of a memory region
    

### make<a name="make"></a>
!!! function "static AnyFile &#42;make(const fs::path &amp;path)"
    
    Creates a media file from a file on disk. The file type is determined automatically
    

!!! function "static AnyFile &#42;make(const fs::path &amp;path, FileType type)"
    
    Creates a media file of a given type from a file on disk
    

!!! function "static AnyFile &#42;make(const u8 &#42;buf, isize len, FileType type)"
    
    Creates a media file of a given type from a memory buffer
    

!!! function "static AnyFile &#42;make(class DriveAPI &amp;drive, FileType type)"
    
    Creates a media file of a given type from the contents of a drive
    

### name<a name="name"></a>
!!! function "virtual string name() const"
    
    Returns the logical name of this file
    

### strip<a name="strip"></a>
!!! function "void strip(isize count)"
    
    Removes a certain number of bytes from the beginning of the file
    

### type<a name="type"></a>
!!! function "static FileType type(const fs::path &amp;path)"
    
    Determines the type of an arbitrary file on disk
    

!!! function "virtual FileType type() const"
    
    Returns the media type of this file
    

### writeToBuffer<a name="writeToBuffer"></a>
!!! function "isize writeToBuffer(u8 &#42;buf, isize offset, isize len)"
    
    Writes a portion of the file to a memory buffer
    

!!! function "isize writeToBuffer(Buffer&lt;u8&gt; &amp;buffer, isize offset, isize len)"
    
    Writes a portion of the file to a buffer
    

!!! function "isize writeToBuffer(u8 &#42;buf)"
    
    Writes the file to a memory buffer
    

!!! function "isize writeToBuffer(Buffer&lt;u8&gt; &amp;buffer)"
    
    Writes the file to a buffer
    

### writeToFile<a name="writeToFile"></a>
!!! function "isize writeToFile(const std::filesystem::path &amp;path, isize offset, isize len)"
    
    Writes a portion of the file to a file on disk
    

!!! function "isize writeToFile(const std::filesystem::path &amp;path)"
    
    Writes the file to a file on disk
    

### writeToStream<a name="writeToStream"></a>
!!! function "isize writeToStream(std::ostream &amp;stream, isize offset, isize len)"
    
    Writes a portion of the file to a stream
    

!!! function "isize writeToStream(std::ostream &amp;stream)"
    
    Writes the file to a stream
    

### ~AnyFile<a name="_u007eAnyFile"></a>
!!! function "virtual ~AnyFile()"
    
    Destructor
    

