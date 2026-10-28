---
generator: doxide
---


# SIDAPI

**`class SIDAPI : public API`**

SID Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [getInfo](#getInfo) |  Returns the current state of a specific SID. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [draw](#draw) | @name Visualizing waveforms @{ Draws the waveform of a SID chip or of the audio output. |

## Function Details

### draw<a name="draw"></a>
!!! function "float draw(u32 &#42;buffer, isize width, isize height, float maxAmp, u32 color, isize sid = -1) const"
    
    @name Visualizing waveforms
    @{
     Draws the waveform of a SID chip or of the audio output.
    
     The waveform is plotted into a texture buffer. The function returns the
     highest amplitude found. To implement auto-scaling, pass the returned
     value as parameter 'maxAmp' in the next call.
    
     
    :material-location-enter: `buffer`
    :     Texture buffer of size width * height
     
    :material-location-enter: `width`
    :      Width of the texture in pixels
     
    :material-location-enter: `height`
    :     Height of the texture in pixels
     
    :material-location-enter: `maxAmp`
    :     Amplitude that is mapped to the full height
     
    :material-location-enter: `color`
    :      Color of the waveform
     
    :material-location-enter: `sid`
    :        Number of the SID chip (0 - 3). Any other value draws
                     the stereo audio output, with the left channel in the
                     upper and the right channel in the lower half.
     
    :material-keyboard-return: **Return**
    :    The highest amplitude that was found.
    

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const SIDInfo &amp;getCachedInfo(isize nr) const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getInfo<a name="getInfo"></a>
!!! function "const SIDInfo &amp;getInfo(isize nr) const"
    
     Returns the current state of a specific SID.
     
    :material-location-enter: `nr`
    :         SID number (0 - 3). 0 is the primary SID.
    
     getInfo() reads the live state on the calling thread and is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    
     
    :material-alert-circle-outline: **Throw**
    :     CoreError (CoreError::OPT_INV_ARG) if the SID number is out of range.
    

