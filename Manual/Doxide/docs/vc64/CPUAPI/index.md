---
generator: doxide
---


# CPUAPI

**`class CPUAPI : public API`**

CPU API


## Variables

| Name | Description |
| ---- | ----------- |
| [debugger](#debugger) | CPU debugger  |
| [breakpoints](#breakpoints) | Breakpoints  |
| [watchpoints](#watchpoints) | Watchpoints  |

## Functions

| Name | Description |
| ---- | ----------- |
| [getConfig](#getConfig) |  Returns the component's current configuration. |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |

## Variable Details

### breakpoints<a name="breakpoints"></a>

!!! variable "GuardsAPI breakpoints"
    
    Breakpoints
    

### debugger<a name="debugger"></a>

!!! variable "CPUDebuggerAPI debugger"
    
    CPU debugger
    

### watchpoints<a name="watchpoints"></a>

!!! variable "GuardsAPI watchpoints"
    
    Watchpoints
    

## Function Details

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const CPUInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getConfig<a name="getConfig"></a>
!!! function "const CPUConfig &amp;getConfig() const"
    
     Returns the component's current configuration.
    

### getInfo<a name="getInfo"></a>
!!! function "const CPUInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

