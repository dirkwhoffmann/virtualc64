---
generator: doxide
---


# PaddleAPI

**`class PaddleAPI : public API`**

Paddle Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |

## Function Details

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const PaddleInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getInfo<a name="getInfo"></a>
!!! function "const PaddleInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

