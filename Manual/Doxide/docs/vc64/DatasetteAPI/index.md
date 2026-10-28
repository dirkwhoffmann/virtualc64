---
generator: doxide
---


# DatasetteAPI

**`class DatasetteAPI : public API`**

Datasette Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [insertTape](#insertTape) |  Inserts a tape. |
| [insertTapeAsync](#insertTapeAsync) |  Asynchronous version of insertTape(): queues the request and returns at once. The file is read on the emulator thread. Success and failure are both reported by the usual message (e.g., CRT_ATTACHED). If the request failed, the message carries the exception.  |
| [ejectTape](#ejectTape) |  Ejects a tape. |
| [exportTape](#exportTape) |  Export tape to a file. |

## Function Details

### ejectTape<a name="ejectTape"></a>
!!! function "void ejectTape()"
    
     Ejects a tape.
     This function has no effect if no tape is inserted.
    

### exportTape<a name="exportTape"></a>
!!! function "void exportTape(const std::filesystem::path &amp;path) const"
    
     Export tape to a file.
     
    :material-location-enter: `path`
    :       Export path.
    

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const DatasetteInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getInfo<a name="getInfo"></a>
!!! function "const DatasetteInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### insertTape<a name="insertTape"></a>
!!! function "void insertTape(const std::filesystem::path &amp;path)"
    
     Inserts a tape.
     
    :material-location-enter: `path`
    :       The tape to insert.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be read or does not contain valid data.
    

### insertTapeAsync<a name="insertTapeAsync"></a>
!!! function "void insertTapeAsync(const std::filesystem::path &amp;path)"
    
     Asynchronous version of insertTape(): queues the request and returns at once.
    
     The file is read on the emulator thread. Success and failure are both
     reported by the usual message (e.g., CRT_ATTACHED). If the request
     failed, the message carries the exception.
    

