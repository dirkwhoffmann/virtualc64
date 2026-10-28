---
generator: doxide
---


# Error

**`class Error : public GenericException<i64>`**

 Base class of the errors thrown by the API

 The payload is an error code. Derived classes (CoreError, IOError,
 MediaError) define the codes and a human-readable message.


## Functions

| Name | Description |
| ---- | ----------- |
| [Error](#Error) | Creates an error with an error code and a message  |
| [fault](#fault) | Returns the error code  |
| [errstr](#errstr) | Returns the name of the error code  |

## Function Details

### Error<a name="Error"></a>
!!! function "Error(i64 d = 0, const std::string &amp;s = &quot;&quot;)"
    
    Creates an error with an error code and a message
    

### errstr<a name="errstr"></a>
!!! function "virtual const char &#42;errstr() const noexcept"
    
    Returns the name of the error code
    

### fault<a name="fault"></a>
!!! function "virtual i64 fault() const"
    
    Returns the error code
    

