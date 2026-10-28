---
generator: doxide
---


# ExpansionPortAPI

**`class ExpansionPortAPI : public API`**

Expansion Port Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [getCartridgeTraits](#getCartridgeTraits) | @{ @name Analyzing cartridges. |
| [getInfo](#getInfo) |  Returns the state of the current cartridge. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [getRomInfo](#getRomInfo) |  Returns the state of one of the cartridge ROM packets. |
| [attachCartridge](#attachCartridge) | @} @name Attaching and detaching cartridges. |
| [attachCartridgeAsync](#attachCartridgeAsync) |  Asynchronous version of attachCartridge(): queues the request and returns at once. The file is read on the emulator thread. Success and failure are both reported by the usual message (e.g., CRT_ATTACHED). If the request failed, the message carries the exception.  |
| [attachReu](#attachReu) |  Attaches a RAM Expansion Unit to the expansion port. |
| [attachGeoRam](#attachGeoRam) |  Attaches a GeoRAM module to the expansion port. |
| [attachIsepicCartridge](#attachIsepicCartridge) |  Attaches an Isepic module to the expansion port. |
| [detachCartridge](#detachCartridge) |  Detaches the currently plugged in cartridge  |
| [exportCRT](#exportCRT) |  Export cartridge  |

## Function Details

### attachCartridge<a name="attachCartridge"></a>
!!! function "void attachCartridge(const std::filesystem::path &amp;path, bool reset = true)"
    
    @}
    @name Attaching and detaching cartridges.
    @{
     Attaches a cartridge to the expansion port.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be read or does not contain valid data.
    

### attachCartridgeAsync<a name="attachCartridgeAsync"></a>
!!! function "void attachCartridgeAsync(const std::filesystem::path &amp;path, bool reset = true)"
    
     Asynchronous version of attachCartridge(): queues the request and returns at once.
    
     The file is read on the emulator thread. Success and failure are both
     reported by the usual message (e.g., CRT_ATTACHED). If the request
     failed, the message carries the exception.
    

### attachGeoRam<a name="attachGeoRam"></a>
!!! function "void attachGeoRam(isize capacity)"
    
     Attaches a GeoRAM module to the expansion port.
    

### attachIsepicCartridge<a name="attachIsepicCartridge"></a>
!!! function "void attachIsepicCartridge()"
    
     Attaches an Isepic module to the expansion port.
    

### attachReu<a name="attachReu"></a>
!!! function "void attachReu(isize capacity)"
    
     Attaches a RAM Expansion Unit to the expansion port.
    

### detachCartridge<a name="detachCartridge"></a>
!!! function "void detachCartridge()"
    
     Detaches the currently plugged in cartridge
    

### exportCRT<a name="exportCRT"></a>
!!! function "void exportCRT(const std::filesystem::path &amp;path) const"
    
     Export cartridge
    

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const CartridgeInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getCartridgeTraits<a name="getCartridgeTraits"></a>
!!! function "CartridgeTraits getCartridgeTraits() const"
    
    @{
    @name Analyzing cartridges.
     Returns the traits of the current cartridge.
    

### getInfo<a name="getInfo"></a>
!!! function "const CartridgeInfo &amp;getInfo() const"
    
     Returns the state of the current cartridge.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### getRomInfo<a name="getRomInfo"></a>
!!! function "CartridgeRomInfo getRomInfo(isize nr) const"
    
     Returns the state of one of the cartridge ROM packets.
     
    :material-location-enter: `nr`
    :         Number of the ROM packet.
    

