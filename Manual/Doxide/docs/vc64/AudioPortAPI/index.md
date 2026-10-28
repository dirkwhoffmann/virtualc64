---
generator: doxide
---


# AudioPortAPI

**`class AudioPortAPI : public API`**

@}
Audio Port Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [getConfig](#getConfig) |  Returns the component's current configuration. |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [getMetrics](#getMetrics) |  Returns statistical information about the components. |
| [getCachedMetrics](#getCachedMetrics) | Returns the most recent metrics snapshot recorded by the emulator thread (see getMetrics()). |
| [copyMono](#copyMono) | @name Retrieving audio data @{ Extracts a number of mono samples from the audio buffer Internally, the audio port maintains a ringbuffer storing stereo audio samples. |
| [copyStereo](#copyStereo) |  Extracts a number of stereo samples from the audio buffer. |
| [copyInterleaved](#copyInterleaved) |  Extracts a number of stereo samples from the audio buffer. |
| [drawL](#drawL) | @name Visualizing waveforms @{ Draws the waveform of the left or the right audio channel. |
| [drawR](#drawR) | Draws the waveform of the right audio channel (see drawL()). |

## Function Details

### copyInterleaved<a name="copyInterleaved"></a>
!!! function "isize copyInterleaved(float &#42;buffer, isize n)"
    
     Extracts a number of stereo samples from the audio buffer.
     This function has to be used if a stereo stream is managed in a
     single destination buffer. The samples of both channels will be
     interleaved, that is, a sample for the left channel will be
     followed by a sample of the right channel and vice versa.
     
    :material-location-enter: `buffer`
    :     Pointer to the destinationleft buffer.
     
    :material-location-enter: `n`
    :          Number of sound samples to copy.
     
    :material-keyboard-return: **Return**
    :            Number of actually copied sound sound samples.
    

### copyMono<a name="copyMono"></a>
!!! function "isize copyMono(float &#42;buffer, isize n)"
    
    @name Retrieving audio data
    @{
     Extracts a number of mono samples from the audio buffer
     Internally, the audio port maintains a ringbuffer storing stereo
     audio samples. When this function is used, both internal stream are
     added together and written to to the destination buffer.
     
    :material-location-enter: `buffer`
    :     Pointer to the destination buffer
     
    :material-location-enter: `n`
    :          Number of sound samples to copy.
     
    :material-keyboard-return: **Return**
    :            Number of actually copied sound sound samples.
    

### copyStereo<a name="copyStereo"></a>
!!! function "isize copyStereo(float &#42;left, float &#42;right, isize n)"
    
     Extracts a number of stereo samples from the audio buffer.
     
    :material-location-enter: `left`
    :       Pointer to the left channel's destination buffer.
     
    :material-location-enter: `right`
    :      Pointer to the right channel's destination buffer.
     
    :material-location-enter: `n`
    :          Number of sound samples to copy.
     
    :material-keyboard-return: **Return**
    :            Number of actually copied sound sound samples.
    

### drawL<a name="drawL"></a>
!!! function "float drawL(u32 &#42;buffer, isize width, isize height, float maxAmp, u32 color) const"
    
    @name Visualizing waveforms
    @{
     Draws the waveform of the left or the right audio channel.
    
     The waveform is plotted into a texture buffer. The functions return the
     highest amplitude found in the audio buffer. To implement auto-scaling,
     pass the returned value as parameter 'maxAmp' in the next call. The
     Mac app uses these functions to visualize the contents of the audio
     buffer in one of its inspector panels.
    
     
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
     
    :material-keyboard-return: **Return**
    :    The highest amplitude that was found.
    

### drawR<a name="drawR"></a>
!!! function "float drawR(u32 &#42;buffer, isize width, isize height, float maxAmp, u32 color) const"
    
    Draws the waveform of the right audio channel (see drawL()).
    

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const AudioPortInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getCachedMetrics<a name="getCachedMetrics"></a>
!!! function "const AudioPortMetrics &amp;getCachedMetrics() const"
    
    Returns the most recent metrics snapshot recorded by the emulator thread (see getMetrics()).
    

### getConfig<a name="getConfig"></a>
!!! function "const AudioPortConfig &amp;getConfig() const"
    
     Returns the component's current configuration.
    

### getInfo<a name="getInfo"></a>
!!! function "const AudioPortInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### getMetrics<a name="getMetrics"></a>
!!! function "const AudioPortMetrics &amp;getMetrics() const"
    
     Returns statistical information about the components.
    
     getMetrics() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedMetrics()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

