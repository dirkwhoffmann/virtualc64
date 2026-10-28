---
generator: doxide
---


# MemoryDebuggerAPI

**`class MemoryDebuggerAPI : public API`**

Memory Debugger API


## Functions

| Name | Description |
| ---- | ----------- |
| [ascDump](#ascDump) | @name Debugging memory @{ Returns a string of characters for a portion of memory. |
| [hexDump](#hexDump) |  Returns a string of numbers for a portion of memory. |
| [decDump](#decDump) | Variant of hexDump() that uses a different output format. |
| [memDump](#memDump) | Variant of hexDump() that uses a different output format. |

## Function Details

### ascDump<a name="ascDump"></a>
!!! function "string ascDump(MemType src, u16 addr, isize bytes) const"
    
    @name Debugging memory
    @{
     Returns a string of characters for a portion of memory.
    
     Unmapped memory (MemType::NONE) is shown as nothing.
    
     
    :material-location-enter: `src`
    :        The memory type the bytes are read from.
     
    :material-location-enter: `addr`
    :       First address to dump.
     
    :material-location-enter: `bytes`
    :      Number of bytes to dump.
    

### decDump<a name="decDump"></a>
!!! function "string decDump(MemType src, u16 addr, isize bytes, isize sz = 1) const"
    
    Variant of hexDump() that uses a different output format.
    

### hexDump<a name="hexDump"></a>
!!! function "string hexDump(MemType src, u16 addr, isize bytes, isize sz = 1) const"
    
     Returns a string of numbers for a portion of memory.
    
     hexDump() shows the bytes as hexadecimal numbers, decDump() as decimal
     numbers, and memDump() as hexadecimal numbers followed by characters.
     Each group of 'sz' bytes forms one number and is followed by a space.
     Unmapped memory (MemType::NONE) is shown as '--'.
    
     
    :material-location-enter: `src`
    :        The memory type the bytes are read from.
     
    :material-location-enter: `addr`
    :       First address to dump.
     
    :material-location-enter: `bytes`
    :      Number of bytes to dump.
     
    :material-location-enter: `sz`
    :         Group size in bytes (1, 2, or 4).
    

### memDump<a name="memDump"></a>
!!! function "string memDump(MemType src, u16 addr, isize bytes, isize sz = 1) const"
    
    Variant of hexDump() that uses a different output format.
    

