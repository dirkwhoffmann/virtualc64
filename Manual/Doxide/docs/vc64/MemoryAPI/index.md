---
generator: doxide
---


# MemoryAPI

**`class MemoryAPI : public API`**

@}
Memory API


## Variables

| Name | Description |
| ---- | ----------- |
| [debugger](#debugger) | Memory debugger  |

## Functions

| Name | Description |
| ---- | ----------- |
| [getConfig](#getConfig) |  Returns the component's current configuration. |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [drawHeatmap](#drawHeatmap) |  Draws a heatmap of the recent memory accesses. |

## Variable Details

### debugger<a name="debugger"></a>

!!! variable "MemoryDebuggerAPI debugger"
    
    Memory debugger
    

## Function Details

### drawHeatmap<a name="drawHeatmap"></a>
!!! function "void drawHeatmap(u32 &#42;buffer, isize width, isize height) const"
    
     Draws a heatmap of the recent memory accesses.
    
     The 64 KB address space is shown as a grid of 256 x 256 cells. Neighboring
     addresses are mapped to neighboring cells by interleaving the address
     bits, so that address ranges form compact blocks. A cell lights up when
     its address is accessed and fades out over time. The map is updated at
     the end of each frame, and only while the option Opt::MEM_HEATMAP is
     enabled.
    
     
    :material-location-enter: `buffer`
    :     Texture buffer for 256 x 256 pixels (RGBA)
     
    :material-location-enter: `width`
    :      Width of the texture, must be 256
     
    :material-location-enter: `height`
    :     Height of the texture, must be 256
    
     
    :material-alert-circle-outline: **Throw**
    :     CoreError (CoreError::OPT_INV_ARG) if the size is not 256 x 256.
    

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const MemInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getConfig<a name="getConfig"></a>
!!! function "const MemConfig &amp;getConfig() const"
    
     Returns the component's current configuration.
    

### getInfo<a name="getInfo"></a>
!!! function "const MemInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

