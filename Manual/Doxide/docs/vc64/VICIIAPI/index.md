---
generator: doxide
---


# VICIIAPI

**`class VICIIAPI : public API`**

VICII API


## Functions

| Name | Description |
| ---- | ----------- |
| [getTraits](#getTraits) |  Provides details about the currently selected VICII revision. |
| [getConfig](#getConfig) |  Returns the component's current configuration. |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [getSpriteInfo](#getSpriteInfo) |  Returns information about a sprite. |
| [getColor](#getColor) |  Returns the RGBA value of a color  |
| [getColor](#getColor) |  Returns the RGBA value of a color from a color palette. |

## Function Details

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const VICIIInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getColor<a name="getColor"></a>
!!! function "u32 getColor(isize nr) const"
    
     Returns the RGBA value of a color
    

!!! function "u32 getColor(isize nr, Palette palette) const"
    
     Returns the RGBA value of a color from a color palette.
    

### getConfig<a name="getConfig"></a>
!!! function "const VICIIConfig &amp;getConfig() const"
    
     Returns the component's current configuration.
    

### getInfo<a name="getInfo"></a>
!!! function "const VICIIInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### getSpriteInfo<a name="getSpriteInfo"></a>
!!! function "SpriteInfo getSpriteInfo(isize nr) const"
    
     Returns information about a sprite.
     
    :material-location-enter: `nr`
    :      Number of the sprite (0 .. 7)
    

### getTraits<a name="getTraits"></a>
!!! function "const VICIITraits &amp;getTraits() const"
    
     Provides details about the currently selected VICII revision.
    

