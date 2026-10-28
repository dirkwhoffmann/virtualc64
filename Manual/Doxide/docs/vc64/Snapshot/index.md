---
generator: doxide
---


# Snapshot

**`class Snapshot : public AnyFile`**

A snapshot of the emulator state

A snapshot contains the complete internal state of the emulator together with a
preview image. Snapshots are created with C64API::takeSnapshot() and can be
restored with C64API::loadSnapshot().


## Functions

| Name | Description |
| ---- | ----------- |
| [isCompatible](#isCompatible) | Checks whether a file, a buffer or a memory region contains a snapshot  |
| [Snapshot](#Snapshot) | Creates a copy of a snapshot  |
| [Snapshot](#Snapshot) | Reads a snapshot from a file  |
| [Snapshot](#Snapshot) | Reads a snapshot from a memory buffer  |
| [Snapshot](#Snapshot) | Creates an empty snapshot with the given capacity  |
| [Snapshot](#Snapshot) | Takes a snapshot of the emulator state  |
| [Snapshot](#Snapshot) | Takes a snapshot of the emulator state and compresses it  |
| [previewImageSize](#previewImageSize) | Returns the size of the preview image (width, height)  |
| [previewImageData](#previewImageData) | Returns the pixel data of the preview image  |
| [timestamp](#timestamp) | Returns the time the snapshot was taken  |
| [isTooOld](#isTooOld) | Returns true if the snapshot was created by an older, incompatible version  |
| [isTooNew](#isTooNew) | Returns true if the snapshot was created by a newer version  |
| [isBeta](#isBeta) | Returns true if the snapshot was created by a beta version  |
| [matches](#matches) | Returns true if the snapshot can be restored by this version  |
| [getHeader](#getHeader) | Returns a pointer to the snapshot header  |
| [getThumbnail](#getThumbnail) | Returns the preview image  |
| [getSnapshotData](#getSnapshotData) | Returns a pointer to the core data  |
| [takeScreenshot](#takeScreenshot) | Records a screenshot  |
| [compressor](#compressor) | Returns the compression method  |
| [isCompressed](#isCompressed) | Returns true if the snapshot is compressed  |
| [compress](#compress) | Compresses the snapshot with the given method  |
| [uncompress](#uncompress) | Uncompresses the snapshot  |

## Function Details

### Snapshot<a name="Snapshot"></a>
!!! function "Snapshot(const Snapshot &amp;other)"
    
    Creates a copy of a snapshot
    

!!! function "Snapshot(const fs::path &amp;path)"
    
    Reads a snapshot from a file
    

!!! function "Snapshot(const u8 &#42;buf, isize len)"
    
    Reads a snapshot from a memory buffer
    

!!! function "Snapshot(isize capacity)"
    
    Creates an empty snapshot with the given capacity
    

!!! function "Snapshot(C64 &amp;c64)"
    
    Takes a snapshot of the emulator state
    

!!! function "Snapshot(C64 &amp;c64, utl::Compressor compressor)"
    
    Takes a snapshot of the emulator state and compresses it
    

### compress<a name="compress"></a>
!!! function "void compress(utl::Compressor method)"
    
    Compresses the snapshot with the given method
    

### compressor<a name="compressor"></a>
!!! function "utl::Compressor compressor() const"
    
    Returns the compression method
    

### getHeader<a name="getHeader"></a>
!!! function "SnapshotHeader &#42;getHeader() const"
    
    Returns a pointer to the snapshot header
    

### getSnapshotData<a name="getSnapshotData"></a>
!!! function "u8 &#42;getSnapshotData() const"
    
    Returns a pointer to the core data
    

### getThumbnail<a name="getThumbnail"></a>
!!! function "const Thumbnail &amp;getThumbnail() const"
    
    Returns the preview image
    

### isBeta<a name="isBeta"></a>
!!! function "bool isBeta() const"
    
    Returns true if the snapshot was created by a beta version
    

### isCompatible<a name="isCompatible"></a>
!!! function "static bool isCompatible(const fs::path &amp;path)"
    
    Checks whether a file, a buffer or a memory region contains a snapshot
    

### isCompressed<a name="isCompressed"></a>
!!! function "bool isCompressed() const"
    
    Returns true if the snapshot is compressed
    

### isTooNew<a name="isTooNew"></a>
!!! function "bool isTooNew() const"
    
    Returns true if the snapshot was created by a newer version
    

### isTooOld<a name="isTooOld"></a>
!!! function "bool isTooOld() const"
    
    Returns true if the snapshot was created by an older, incompatible version
    

### matches<a name="matches"></a>
!!! function "bool matches()"
    
    Returns true if the snapshot can be restored by this version
    

### previewImageData<a name="previewImageData"></a>
!!! function "const u32 &#42;previewImageData() const"
    
    Returns the pixel data of the preview image
    

### previewImageSize<a name="previewImageSize"></a>
!!! function "std::pair &lt;isize,isize&gt; previewImageSize() const"
    
    Returns the size of the preview image (width, height)
    

### takeScreenshot<a name="takeScreenshot"></a>
!!! function "void takeScreenshot(C64 &amp;c64)"
    
    Records a screenshot
    

### timestamp<a name="timestamp"></a>
!!! function "time_t timestamp() const"
    
    Returns the time the snapshot was taken
    

### uncompress<a name="uncompress"></a>
!!! function "void uncompress()"
    
    Uncompresses the snapshot
    

