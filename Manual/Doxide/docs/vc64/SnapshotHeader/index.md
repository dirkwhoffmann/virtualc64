---
generator: doxide
---


# SnapshotHeader

**`struct SnapshotHeader`**

Header of a snapshot


## Variables

| Name | Description |
| ---- | ----------- |
| [magic](#magic) | Magic bytes ('V','C','6','4')  |
| [major](#major) | Major version number  |
| [minor](#minor) | Minor version number  |
| [subminor](#subminor) | Subminor version number  |
| [beta](#beta) | Beta release number  |
| [rawSize](#rawSize) | Size of this snapshot when uncompressed  |
| [compressor](#compressor) | Applied compression method  |
| [screenshot](#screenshot) | Preview image  |

## Variable Details

### beta<a name="beta"></a>

!!! variable "u8 beta"
    
    Beta release number
    

### compressor<a name="compressor"></a>

!!! variable "u8 compressor"
    
    Applied compression method
    

### magic<a name="magic"></a>

!!! variable "char magic[4]"
    
    Magic bytes ('V','C','6','4')
    

### major<a name="major"></a>

!!! variable "u8 major"
    
    Major version number
    

### minor<a name="minor"></a>

!!! variable "u8 minor"
    
    Minor version number
    

### rawSize<a name="rawSize"></a>

!!! variable "i32 rawSize"
    
    Size of this snapshot when uncompressed
    

### screenshot<a name="screenshot"></a>

!!! variable "Thumbnail screenshot"
    
    Preview image
    

### subminor<a name="subminor"></a>

!!! variable "u8 subminor"
    
    Subminor version number
    

