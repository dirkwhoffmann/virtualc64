---
generator: doxide
---


# CoreError

**`struct CoreError : public utl::Error`**

 Exception thrown if a request does not match the state of the emulator

 A CoreError is thrown, for example, if a configuration option is set to an
 invalid value, if a ROM is missing, or if the emulator thread cannot carry
 out a request. The error code is stored in the payload and is one of the
 constants below. It can be queried with the inherited member functions.


## Variables

| Name | Description |
| ---- | ----------- |
| [OK](#OK) | No error  |
| [UNKNOWN](#UNKNOWN) | Unclassified error condition  |
| [LAUNCH_FAILED](#LAUNCH_FAILED) | Launching the emulator thread failed  |
| [NOT_LAUNCHED](#NOT_LAUNCHED) | The emulator thread is not running  |
| [ALREADY_LAUNCHED](#ALREADY_LAUNCHED) | The emulator thread has been launched already  |
| [POWERED_OFF](#POWERED_OFF) | The emulator is powered off  |
| [POWERED_ON](#POWERED_ON) | The emulator is powered on  |
| [RUNNING](#RUNNING) | The emulator is running  |
| [OPT_UNSUPPORTED](#OPT_UNSUPPORTED) | Unsupported configuration option  |
| [OPT_INV_ARG](#OPT_INV_ARG) | Invalid argument  |
| [OPT_INV_ID](#OPT_INV_ID) | Invalid component id  |
| [OPT_LOCKED](#OPT_LOCKED) | The option is temporarily locked  |
| [INVALID_KEY](#INVALID_KEY) | Invalid property key  |
| [SYNTAX](#SYNTAX) | Syntax error  |
| [GUARD_NOT_FOUND](#GUARD_NOT_FOUND) | Guard is not set  |
| [GUARD_ALREADY_SET](#GUARD_ALREADY_SET) | Guard is already set  |
| [BP_NOT_FOUND](#BP_NOT_FOUND) | Breakpoint is not set (DEPRECATED)  |
| [BP_ALREADY_SET](#BP_ALREADY_SET) | Breakpoint is already set (DEPRECATED)  |
| [WP_NOT_FOUND](#WP_NOT_FOUND) | Watchpoint is not set (DEPRECATED)  |
| [WP_ALREADY_SET](#WP_ALREADY_SET) | Watchpoint is already set (DEPRECATED)  |
| [OUT_OF_MEMORY](#OUT_OF_MEMORY) | Out of memory  |
| [ROM_BASIC_MISSING](#ROM_BASIC_MISSING) | No Basic Rom installed  |
| [ROM_CHAR_MISSING](#ROM_CHAR_MISSING) | No Character Rom installed  |
| [ROM_KERNAL_MISSING](#ROM_KERNAL_MISSING) | No Kernal Rom installed  |
| [ROM_DRIVE_MISSING](#ROM_DRIVE_MISSING) | No Floppy Drive Rom installed  |
| [ROM_MEGA65_MISMATCH](#ROM_MEGA65_MISMATCH) | MEGA65 Roms have different version numbers  |
| [DRV_UNCONNECTED](#DRV_UNCONNECTED) | Floppy drive is not connected  |
| [DRV_NO_DISK](#DRV_NO_DISK) | Floppy drive contains no disk  |

## Functions

| Name | Description |
| ---- | ----------- |
| [errstr](#errstr) | Returns the name of the error code  |
| [CoreError](#CoreError) | Creates an error with an error code and a descriptive string  |
| [CoreError](#CoreError) | Creates an error with an error code and a descriptive string  |
| [CoreError](#CoreError) | Creates an error with an error code and a file path  |
| [CoreError](#CoreError) | Creates an error with an error code and a number  |
| [CoreError](#CoreError) | Creates an error with an error code  |

## Variable Details

### ALREADY_LAUNCHED<a name="ALREADY_LAUNCHED"></a>

!!! variable "static constexpr long ALREADY_LAUNCHED"
    
    The emulator thread has been launched already
    

### BP_ALREADY_SET<a name="BP_ALREADY_SET"></a>

!!! variable "static constexpr long BP_ALREADY_SET"
    
    Breakpoint is already set (DEPRECATED)
    

### BP_NOT_FOUND<a name="BP_NOT_FOUND"></a>

!!! variable "static constexpr long BP_NOT_FOUND"
    
    Breakpoint is not set (DEPRECATED)
    

### DRV_NO_DISK<a name="DRV_NO_DISK"></a>

!!! variable "static constexpr long DRV_NO_DISK"
    
    Floppy drive contains no disk
    

### DRV_UNCONNECTED<a name="DRV_UNCONNECTED"></a>

!!! variable "static constexpr long DRV_UNCONNECTED"
    
    Floppy drive is not connected
    

### GUARD_ALREADY_SET<a name="GUARD_ALREADY_SET"></a>

!!! variable "static constexpr long GUARD_ALREADY_SET"
    
    Guard is already set
    

### GUARD_NOT_FOUND<a name="GUARD_NOT_FOUND"></a>

!!! variable "static constexpr long GUARD_NOT_FOUND"
    
    Guard is not set
    

### INVALID_KEY<a name="INVALID_KEY"></a>

!!! variable "static constexpr long INVALID_KEY"
    
    Invalid property key
    

### LAUNCH_FAILED<a name="LAUNCH_FAILED"></a>

!!! variable "static constexpr long LAUNCH_FAILED"
    
    Launching the emulator thread failed
    

### NOT_LAUNCHED<a name="NOT_LAUNCHED"></a>

!!! variable "static constexpr long NOT_LAUNCHED"
    
    The emulator thread is not running
    

### OK<a name="OK"></a>

!!! variable "static constexpr long OK"
    
    No error
    

### OPT_INV_ARG<a name="OPT_INV_ARG"></a>

!!! variable "static constexpr long OPT_INV_ARG"
    
    Invalid argument
    

### OPT_INV_ID<a name="OPT_INV_ID"></a>

!!! variable "static constexpr long OPT_INV_ID"
    
    Invalid component id
    

### OPT_LOCKED<a name="OPT_LOCKED"></a>

!!! variable "static constexpr long OPT_LOCKED"
    
    The option is temporarily locked
    

### OPT_UNSUPPORTED<a name="OPT_UNSUPPORTED"></a>

!!! variable "static constexpr long OPT_UNSUPPORTED"
    
    Unsupported configuration option
    

### OUT_OF_MEMORY<a name="OUT_OF_MEMORY"></a>

!!! variable "static constexpr long OUT_OF_MEMORY"
    
    Out of memory
    

### POWERED_OFF<a name="POWERED_OFF"></a>

!!! variable "static constexpr long POWERED_OFF"
    
    The emulator is powered off
    

### POWERED_ON<a name="POWERED_ON"></a>

!!! variable "static constexpr long POWERED_ON"
    
    The emulator is powered on
    

### ROM_BASIC_MISSING<a name="ROM_BASIC_MISSING"></a>

!!! variable "static constexpr long ROM_BASIC_MISSING"
    
    No Basic Rom installed
    

### ROM_CHAR_MISSING<a name="ROM_CHAR_MISSING"></a>

!!! variable "static constexpr long ROM_CHAR_MISSING"
    
    No Character Rom installed
    

### ROM_DRIVE_MISSING<a name="ROM_DRIVE_MISSING"></a>

!!! variable "static constexpr long ROM_DRIVE_MISSING"
    
    No Floppy Drive Rom installed
    

### ROM_KERNAL_MISSING<a name="ROM_KERNAL_MISSING"></a>

!!! variable "static constexpr long ROM_KERNAL_MISSING"
    
    No Kernal Rom installed
    

### ROM_MEGA65_MISMATCH<a name="ROM_MEGA65_MISMATCH"></a>

!!! variable "static constexpr long ROM_MEGA65_MISMATCH"
    
    MEGA65 Roms have different version numbers
    

### RUNNING<a name="RUNNING"></a>

!!! variable "static constexpr long RUNNING"
    
    The emulator is running
    

### SYNTAX<a name="SYNTAX"></a>

!!! variable "static constexpr long SYNTAX"
    
    Syntax error
    

### UNKNOWN<a name="UNKNOWN"></a>

!!! variable "static constexpr long UNKNOWN"
    
    Unclassified error condition
    

### WP_ALREADY_SET<a name="WP_ALREADY_SET"></a>

!!! variable "static constexpr long WP_ALREADY_SET"
    
    Watchpoint is already set (DEPRECATED)
    

### WP_NOT_FOUND<a name="WP_NOT_FOUND"></a>

!!! variable "static constexpr long WP_NOT_FOUND"
    
    Watchpoint is not set (DEPRECATED)
    

## Function Details

### CoreError<a name="CoreError"></a>
!!! function "explicit CoreError(long fault, const string &amp;s)"
    
    Creates an error with an error code and a descriptive string
    

!!! function "explicit CoreError(long fault, const char &#42;s)"
    
    Creates an error with an error code and a descriptive string
    

!!! function "explicit CoreError(long fault, const fs::path &amp;p)"
    
    Creates an error with an error code and a file path
    

!!! function "explicit CoreError(long fault, std::integral auto v)"
    
    Creates an error with an error code and a number
    

!!! function "explicit CoreError(long fault)"
    
    Creates an error with an error code
    

### errstr<a name="errstr"></a>
!!! function "const char &#42;errstr() const noexcept override"
    
    Returns the name of the error code
    

