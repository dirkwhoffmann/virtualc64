---
generator: doxide
---


# VideoPortAPI

**`class VideoPortAPI : public API`**

@}
Video Port Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [getConfig](#getConfig) |  Returns the component's current configuration. |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [lockTexture](#lockTexture) | @name Retrieving video data @{ Locks the emulator texture This function aquires a mutex that prevents the emulator to modify the stable texture. |
| [unlockTexture](#unlockTexture) |  Unlocks the emulator texture This function releases the mutex acquired in lockTexture(). |
| [getTexture](#getTexture) |  Returns a pointer to the most recent stable texture The texture dimensions are given by constants vc64::Tex::width and vc64::Tex::height texels. Each texel is represented by a 32 bit color value.  |
| [getTexture](#getTexture) |  Returns the most recent stable texture together with its frame number :material-location-enter: `nr` :         Receives the frame number.  |
| [getXrayTexture](#getXrayTexture) |  Returns a pointer to the most recent stable X-Ray texture The texture has the same dimensions as the regular texture. |
| [getXrayTexture](#getXrayTexture) |  Returns the most recent stable X-Ray texture together with its frame number :material-location-enter: `nr` :         Receives the frame number, which matches the number of the regular texture of the same frame.  |
| [saveTexture](#saveTexture) |  Saves the most recent stable texture as a PNG file The cutout is given in texels of the texture. |
| [saveTexture](#saveTexture) | Saves a cutout of the most recent stable texture as a PNG file (see above). |
| [saveXrayTexture](#saveXrayTexture) |  Saves the most recent stable X-Ray texture as a PNG file :material-alert-circle-outline: **Throw** :    Error if the file cannot be written.  |
| [saveXrayTexture](#saveXrayTexture) | Saves a cutout of the most recent stable X-Ray texture as a PNG file. |
| [findInnerArea](#findInnerArea) |  Determines the area of the texture that contains picture data. |
| [findInnerAreaNormalized](#findInnerAreaNormalized) | Same as findInnerArea(), but returns normalized coordinates. |

## Function Details

### findInnerArea<a name="findInnerArea"></a>
!!! function "void findInnerArea(isize &amp;x1, isize &amp;x2, isize &amp;y1, isize &amp;y2) const"
    
     Determines the area of the texture that contains picture data.
    
     The result is meant to be used to crop the border of the picture. The
     VirtualC64 core does not analyze the texture yet: the functions always
     return the complete texture. The emulator thread is suspended during
     the call.
    
     findInnerArea() returns the coordinates in texture pixels,
     findInnerAreaNormalized() returns them relative to the size of the
     texture (0.0 ... 1.0).
    
     
    :material-location-enter: `x1`
    :         Left edge of the picture area
     
    :material-location-enter: `x2`
    :         Right edge of the picture area
     
    :material-location-enter: `y1`
    :         Top edge of the picture area
     
    :material-location-enter: `y2`
    :         Bottom edge of the picture area
    

### findInnerAreaNormalized<a name="findInnerAreaNormalized"></a>
!!! function "void findInnerAreaNormalized(double &amp;x1, double &amp;x2, double &amp;y1, double &amp;y2) const"
    
    Same as findInnerArea(), but returns normalized coordinates.
    

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const VideoPortInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getConfig<a name="getConfig"></a>
!!! function "const VideoPortConfig &amp;getConfig() const"
    
     Returns the component's current configuration.
    

### getInfo<a name="getInfo"></a>
!!! function "const VideoPortInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### getTexture<a name="getTexture"></a>
!!! function "const u32 &#42;getTexture() const"
    
     Returns a pointer to the most recent stable texture
    
    The texture dimensions are given by constants vc64::Tex::width
    and vc64::Tex::height texels. Each texel is represented by a
    32 bit color value.
    

!!! function "const u32 &#42;getTexture(isize &#42;nr) const"
    
     Returns the most recent stable texture together with its frame number
     
    :material-location-enter: `nr`
    :         Receives the frame number.
    

### getXrayTexture<a name="getXrayTexture"></a>
!!! function "const u32 &#42;getXrayTexture() const"
    
     Returns a pointer to the most recent stable X-Ray texture
    
    The texture has the same dimensions as the regular texture.
    

!!! function "const u32 &#42;getXrayTexture(isize &#42;nr) const"
    
     Returns the most recent stable X-Ray texture together with its frame number
     
    :material-location-enter: `nr`
    :         Receives the frame number, which matches the number of
                     the regular texture of the same frame.
    

### lockTexture<a name="lockTexture"></a>
!!! function "void lockTexture()"
    
    @name Retrieving video data
    @{
     Locks the emulator texture
    
    This function aquires a mutex that prevents the emulator to modify the
    stable texture. Call this function prior to getTexture().
    

### saveTexture<a name="saveTexture"></a>
!!! function "void saveTexture(const std::filesystem::path &amp;path) const"
    
     Saves the most recent stable texture as a PNG file
    
    The cutout is given in texels of the texture. The upper bounds x2 and
    y2 are exclusive. Without a cutout, the entire texture is saved.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be written.
    

!!! function "void saveTexture(const std::filesystem::path &amp;path, isize x1, isize x2, isize y1, isize y2) const"
    
    Saves a cutout of the most recent stable texture as a PNG file (see above).
    

### saveXrayTexture<a name="saveXrayTexture"></a>
!!! function "void saveXrayTexture(const std::filesystem::path &amp;path) const"
    
     Saves the most recent stable X-Ray texture as a PNG file
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be written.
    

!!! function "void saveXrayTexture(const std::filesystem::path &amp;path, isize x1, isize x2, isize y1, isize y2) const"
    
    Saves a cutout of the most recent stable X-Ray texture as a PNG file.
    

### unlockTexture<a name="unlockTexture"></a>
!!! function "void unlockTexture()"
    
     Unlocks the emulator texture
    
    This function releases the mutex acquired in lockTexture(). Call this
    function when the pointer returned by getTexture() is no longer needed.
    

