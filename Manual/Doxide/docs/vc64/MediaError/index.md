---
generator: doxide
---


# MediaError

**`struct MediaError : public utl::Error`**

 Exception thrown if a media file is invalid or not supported

 Media files are ROMs, snapshots, cartridges, disk images, and similar
 files. The error code (see fault()) is one of the constants below.


## Variables

| Name | Description |
| ---- | ----------- |
| [OK](#OK) | No error  |
| [UNKNOWN](#UNKNOWN) | Unclassified  |
| [SNAP_TOO_OLD](#SNAP_TOO_OLD) | Snapshot was created with an older version  |
| [SNAP_TOO_NEW](#SNAP_TOO_NEW) | Snapshot was created with a later version  |
| [SNAP_IS_BETA](#SNAP_IS_BETA) | Snapshot was created with a beta release  |
| [SNAP_CORRUPTED](#SNAP_CORRUPTED) | Snapshot data is corrupted  |
| [CRT_NO_CARTRIDGE](#CRT_NO_CARTRIDGE) | No cartridge attached  |
| [CRT_UNKNOWN](#CRT_UNKNOWN) | Unknown cartridge type  |
| [CRT_UNSUPPORTED](#CRT_UNSUPPORTED) | Unsupported cartridge type  |
| [CRT_TOO_MANY_PACKETS](#CRT_TOO_MANY_PACKETS) | CRT file contains too many Rom packets  |
| [CRT_CORRUPTED_PACKET](#CRT_CORRUPTED_PACKET) | CRT file contains a corrupted Rom package  |

## Functions

| Name | Description |
| ---- | ----------- |
| [errstr](#errstr) | Returns the name of the error code  |
| [MediaError](#MediaError) | Creates an error with an error code and a descriptive string  |
| [MediaError](#MediaError) | Creates an error with an error code and a descriptive string  |
| [MediaError](#MediaError) | Creates an error with an error code and a file path  |
| [MediaError](#MediaError) | Creates an error with an error code and a number  |
| [MediaError](#MediaError) | Creates an error with an error code  |

## Variable Details

### CRT_CORRUPTED_PACKET<a name="CRT_CORRUPTED_PACKET"></a>

!!! variable "static constexpr long CRT_CORRUPTED_PACKET"
    
    CRT file contains a corrupted Rom package
    

### CRT_NO_CARTRIDGE<a name="CRT_NO_CARTRIDGE"></a>

!!! variable "static constexpr long CRT_NO_CARTRIDGE"
    
    No cartridge attached
    

### CRT_TOO_MANY_PACKETS<a name="CRT_TOO_MANY_PACKETS"></a>

!!! variable "static constexpr long CRT_TOO_MANY_PACKETS"
    
    CRT file contains too many Rom packets
    

### CRT_UNKNOWN<a name="CRT_UNKNOWN"></a>

!!! variable "static constexpr long CRT_UNKNOWN"
    
    Unknown cartridge type
    

### CRT_UNSUPPORTED<a name="CRT_UNSUPPORTED"></a>

!!! variable "static constexpr long CRT_UNSUPPORTED"
    
    Unsupported cartridge type
    

### OK<a name="OK"></a>

!!! variable "static constexpr long OK"
    
    No error
    

### SNAP_CORRUPTED<a name="SNAP_CORRUPTED"></a>

!!! variable "static constexpr long SNAP_CORRUPTED"
    
    Snapshot data is corrupted
    

### SNAP_IS_BETA<a name="SNAP_IS_BETA"></a>

!!! variable "static constexpr long SNAP_IS_BETA"
    
    Snapshot was created with a beta release
    

### SNAP_TOO_NEW<a name="SNAP_TOO_NEW"></a>

!!! variable "static constexpr long SNAP_TOO_NEW"
    
    Snapshot was created with a later version
    

### SNAP_TOO_OLD<a name="SNAP_TOO_OLD"></a>

!!! variable "static constexpr long SNAP_TOO_OLD"
    
    Snapshot was created with an older version
    

### UNKNOWN<a name="UNKNOWN"></a>

!!! variable "static constexpr long UNKNOWN"
    
    Unclassified
    

## Function Details

### MediaError<a name="MediaError"></a>
!!! function "explicit MediaError(long fault, const string &amp;s)"
    
    Creates an error with an error code and a descriptive string
    

!!! function "explicit MediaError(long fault, const char &#42;s)"
    
    Creates an error with an error code and a descriptive string
    

!!! function "explicit MediaError(long fault, const fs::path &amp;p)"
    
    Creates an error with an error code and a file path
    

!!! function "explicit MediaError(long fault, std::integral auto v)"
    
    Creates an error with an error code and a number
    

!!! function "explicit MediaError(long fault)"
    
    Creates an error with an error code
    

### errstr<a name="errstr"></a>
!!! function "const char &#42;errstr() const noexcept override"
    
    Returns the name of the error code
    

