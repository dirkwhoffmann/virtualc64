---
generator: doxide
---


# CIAAPI

**`class CIAAPI : public API`**

CIA API


## Functions

| Name | Description |
| ---- | ----------- |
| [getConfig](#getConfig) |  Returns the component's current configuration. |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [getMetrics](#getMetrics) |  Returns statistical information about the components. |
| [getCachedMetrics](#getCachedMetrics) | Returns the most recent metrics snapshot recorded by the emulator thread (see getMetrics()). |

## Function Details

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const CIAInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getCachedMetrics<a name="getCachedMetrics"></a>
!!! function "const CIAMetrics &amp;getCachedMetrics() const"
    
    Returns the most recent metrics snapshot recorded by the emulator thread (see getMetrics()).
    

### getConfig<a name="getConfig"></a>
!!! function "const CIAConfig &amp;getConfig() const"
    
     Returns the component's current configuration.
    

### getInfo<a name="getInfo"></a>
!!! function "const CIAInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### getMetrics<a name="getMetrics"></a>
!!! function "const CIAMetrics &amp;getMetrics() const"
    
     Returns statistical information about the components.
    
     getMetrics() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedMetrics()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

