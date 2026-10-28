---
generator: doxide
---


# DriveAPI

**`class DriveAPI : public API`**

Drive Public API


## Variables

| Name | Description |
| ---- | ----------- |
| [disk](#disk) | The disk in the drive  |

## Functions

| Name | Description |
| ---- | ----------- |
| [getConfig](#getConfig) |  Returns the component's current configuration. |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [insertBlankDisk](#insertBlankDisk) |  Inserts a new disk. |
| [insert](#insert) |  Inserts a disk created from an image file. |
| [insertAsync](#insertAsync) |  Asynchronous version of insert(): queues the request and returns at once. The file is read on the emulator thread. Success and failure are both reported by the usual message (e.g., CRT_ATTACHED). If the request failed, the message carries the exception.  |
| [ejectDisk](#ejectDisk) |  Ejects the current disk. |
| [save](#save) |  Saves the current disk as an image file. |
| [saveFiles](#saveFiles) |  Exports the current disk to a file. |

## Variable Details

### disk<a name="disk"></a>

!!! variable "DiskAPI disk"
    
    The disk in the drive
    

## Function Details

### ejectDisk<a name="ejectDisk"></a>
!!! function "void ejectDisk()"
    
     Ejects the current disk.
    

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const DriveInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getConfig<a name="getConfig"></a>
!!! function "const DriveConfig &amp;getConfig() const"
    
     Returns the component's current configuration.
    

### getInfo<a name="getInfo"></a>
!!! function "const DriveInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### insert<a name="insert"></a>
!!! function "void insert(const std::filesystem::path&amp; path, bool wp)"
    
     Inserts a disk created from an image file.
     
    :material-location-enter: `path`
    :      Path to the disk image.
     
    :material-location-enter: `wp`
    :         Write-protection status of the disk.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be read or does not contain valid data.
    

### insertAsync<a name="insertAsync"></a>
!!! function "void insertAsync(const std::filesystem::path&amp; path, bool wp)"
    
     Asynchronous version of insert(): queues the request and returns at once.
    
     The file is read on the emulator thread. Success and failure are both
     reported by the usual message (e.g., CRT_ATTACHED). If the request
     failed, the message carries the exception.
    

### insertBlankDisk<a name="insertBlankDisk"></a>
!!! function "void insertBlankDisk(FSFormat fstype, string name)"
    
     Inserts a new disk.
     
    :material-location-enter: `fstype`
    :     The file system the disk should be formatted with.
     
    :material-location-enter: `name`
    :       A PET string with the name of the new disk.
    

### save<a name="save"></a>
!!! function "void save(const std::filesystem::path&amp; path)"
    
     Saves the current disk as an image file.
     
    :material-location-enter: `path`
    :      Path to the destination file.
    

### saveFiles<a name="saveFiles"></a>
!!! function "void saveFiles(const std::filesystem::path&amp; path)"
    
     Exports the current disk to a file.
     
    :material-location-enter: `path`
    :      Path to the destination folder.
    

