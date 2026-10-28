---
generator: doxide
---


# GenericException

**`template<typename T> struct GenericException : public Exception`**

An exception that carries a payload and a message


## Variables

| Name | Description |
| ---- | ----------- |
| [payload](#payload) | Exception-specific payload  |
| [msg](#msg) | Message returned by what()  |

## Functions

| Name | Description |
| ---- | ----------- |
| [what](#what) | Returns the message  |

## Variable Details

### msg<a name="msg"></a>

!!! variable "std::string msg"
    
    Message returned by what()
    

### payload<a name="payload"></a>

!!! variable "T payload"
    
    Exception-specific payload
    

## Function Details

### what<a name="what"></a>
!!! function "const char &#42;what() const noexcept override"
    
    Returns the message
    

