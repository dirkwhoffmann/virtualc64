---
generator: doxide
---


# Thumbnail

**`struct Thumbnail`**

Preview image of a snapshot


## Variables

| Name | Description |
| ---- | ----------- |
| [width](#width) | Image size  |
| [screen](#screen) | Raw texture data  |
| [timestamp](#timestamp) | Creation date and time  |

## Functions

| Name | Description |
| ---- | ----------- |
| [makeWithC64](#makeWithC64) | Factory methods  |
| [take](#take) | Takes a screenshot from the current texture  |

## Variable Details

### screen<a name="screen"></a>

!!! variable "u32 screen[Tex::height &#42; Tex::width]"
    
    Raw texture data
    

### timestamp<a name="timestamp"></a>

!!! variable "time_t timestamp"
    
    Creation date and time
    

### width<a name="width"></a>

!!! variable "i32 width, height"
    
    Image size
    

## Function Details

### makeWithC64<a name="makeWithC64"></a>
!!! function "static Thumbnail &#42;makeWithC64(const C64 &amp;c64, isize dx = 1, isize dy = 1)"
    
    Factory methods
    

### take<a name="take"></a>
!!! function "void take(const C64 &amp;c64, isize dx = 1, isize dy = 1)"
    
    Takes a screenshot from the current texture
    

