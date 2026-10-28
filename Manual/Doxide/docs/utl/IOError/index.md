---
generator: doxide
---


# IOError

**`struct IOError : public Error`**

 Exception thrown if a file or folder cannot be accessed

 The error code (see fault()) is one of the constants below. The message
 returned by what() names the affected file or folder.


## Variables

| Name | Description |
| ---- | ----------- |
| [DIR_NOT_FOUND](#DIR_NOT_FOUND) | The folder does not exist  |
| [DIR_ACCESS_DENIED](#DIR_ACCESS_DENIED) | Access to the folder was denied  |
| [DIR_CANT_CREATE](#DIR_CANT_CREATE) | The folder could not be created  |
| [DIR_CANT_DELETE](#DIR_CANT_DELETE) | The folder could not be deleted  |
| [DIR_NOT_EMPTY](#DIR_NOT_EMPTY) | The folder is not empty  |
| [FILE_NOT_FOUND](#FILE_NOT_FOUND) | The file does not exist  |
| [FILE_EXISTS](#FILE_EXISTS) | The file exists already  |
| [FILE_IS_DIRECTORY](#FILE_IS_DIRECTORY) | The path refers to a folder, not a file  |
| [FILE_ACCESS_DENIED](#FILE_ACCESS_DENIED) | Access to the file was denied  |
| [FILE_TYPE_MISMATCH](#FILE_TYPE_MISMATCH) | The file has the wrong type  |
| [FILE_TYPE_UNSUPPORTED](#FILE_TYPE_UNSUPPORTED) | The file type is not supported  |
| [FILE_CANT_READ](#FILE_CANT_READ) | The file could not be read  |
| [FILE_CANT_WRITE](#FILE_CANT_WRITE) | The file could not be written  |
| [FILE_CANT_CREATE](#FILE_CANT_CREATE) | The file could not be created  |
| [FILE_CANT_DELETE](#FILE_CANT_DELETE) | The file could not be deleted  |
| [FILE_INVALID_DATA](#FILE_INVALID_DATA) | The file contains invalid data  |
| [FILE_IN_USE](#FILE_IN_USE) | The file is in use  |
| [JSON_ERROR](#JSON_ERROR) | A JSON document could not be processed  |
| [ZLIB_ERROR](#ZLIB_ERROR) | Compression or decompression failed  |

## Functions

| Name | Description |
| ---- | ----------- |
| [errstr](#errstr) | Returns the name of the error code  |
| [IOError](#IOError) | Creates an error with an error code and the name of the affected file or folder  |

## Variable Details

### DIR_ACCESS_DENIED<a name="DIR_ACCESS_DENIED"></a>

!!! variable "static constexpr long DIR_ACCESS_DENIED"
    
    Access to the folder was denied
    

### DIR_CANT_CREATE<a name="DIR_CANT_CREATE"></a>

!!! variable "static constexpr long DIR_CANT_CREATE"
    
    The folder could not be created
    

### DIR_CANT_DELETE<a name="DIR_CANT_DELETE"></a>

!!! variable "static constexpr long DIR_CANT_DELETE"
    
    The folder could not be deleted
    

### DIR_NOT_EMPTY<a name="DIR_NOT_EMPTY"></a>

!!! variable "static constexpr long DIR_NOT_EMPTY"
    
    The folder is not empty
    

### DIR_NOT_FOUND<a name="DIR_NOT_FOUND"></a>

!!! variable "static constexpr long DIR_NOT_FOUND"
    
    The folder does not exist
    

### FILE_ACCESS_DENIED<a name="FILE_ACCESS_DENIED"></a>

!!! variable "static constexpr long FILE_ACCESS_DENIED"
    
    Access to the file was denied
    

### FILE_CANT_CREATE<a name="FILE_CANT_CREATE"></a>

!!! variable "static constexpr long FILE_CANT_CREATE"
    
    The file could not be created
    

### FILE_CANT_DELETE<a name="FILE_CANT_DELETE"></a>

!!! variable "static constexpr long FILE_CANT_DELETE"
    
    The file could not be deleted
    

### FILE_CANT_READ<a name="FILE_CANT_READ"></a>

!!! variable "static constexpr long FILE_CANT_READ"
    
    The file could not be read
    

### FILE_CANT_WRITE<a name="FILE_CANT_WRITE"></a>

!!! variable "static constexpr long FILE_CANT_WRITE"
    
    The file could not be written
    

### FILE_EXISTS<a name="FILE_EXISTS"></a>

!!! variable "static constexpr long FILE_EXISTS"
    
    The file exists already
    

### FILE_INVALID_DATA<a name="FILE_INVALID_DATA"></a>

!!! variable "static constexpr long FILE_INVALID_DATA"
    
    The file contains invalid data
    

### FILE_IN_USE<a name="FILE_IN_USE"></a>

!!! variable "static constexpr long FILE_IN_USE"
    
    The file is in use
    

### FILE_IS_DIRECTORY<a name="FILE_IS_DIRECTORY"></a>

!!! variable "static constexpr long FILE_IS_DIRECTORY"
    
    The path refers to a folder, not a file
    

### FILE_NOT_FOUND<a name="FILE_NOT_FOUND"></a>

!!! variable "static constexpr long FILE_NOT_FOUND"
    
    The file does not exist
    

### FILE_TYPE_MISMATCH<a name="FILE_TYPE_MISMATCH"></a>

!!! variable "static constexpr long FILE_TYPE_MISMATCH"
    
    The file has the wrong type
    

### FILE_TYPE_UNSUPPORTED<a name="FILE_TYPE_UNSUPPORTED"></a>

!!! variable "static constexpr long FILE_TYPE_UNSUPPORTED"
    
    The file type is not supported
    

### JSON_ERROR<a name="JSON_ERROR"></a>

!!! variable "static constexpr long JSON_ERROR"
    
    A JSON document could not be processed
    

### ZLIB_ERROR<a name="ZLIB_ERROR"></a>

!!! variable "static constexpr long ZLIB_ERROR"
    
    Compression or decompression failed
    

## Function Details

### IOError<a name="IOError"></a>
!!! function "explicit IOError(long fault, const std::string &amp;msg = &quot;&quot;)"
    
    Creates an error with an error code and the name of the affected file or folder
    

### errstr<a name="errstr"></a>
!!! function "const char &#42;errstr() const noexcept override"
    
    Returns the name of the error code
    

