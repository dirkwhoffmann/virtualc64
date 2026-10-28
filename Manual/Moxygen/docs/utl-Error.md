---
title: "Error"
kind: class
---
# Error

```cpp
#include <utlib/include/utl/types/Exception.h>
```

```cpp
class Error
```

Defined in utlib/include/utl/types/Exception.h:49

Base class of the errors thrown by the API.

The payload is an error code. Derived classes (CoreError, [IOError](utl-IOError.md#ioerror), MediaError) define the codes and a human-readable message.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Error`](#error-1) `inline` | Creates an error with an error code and a message. |
| `i64` | [`fault`](#fault) `virtual` `const` `inline` | Returns the error code. |
| `const char *` | [`errstr`](#errstr) `virtual` `const` `inline` `noexcept` | Returns the name of the error code. |

---

### Error

`inline`

```cpp
inline Error(i64 d = 0, const std::string & s = "")
```

Defined in utlib/include/utl/types/Exception.h:54

Creates an error with an error code and a message.

---

### fault

`virtual` `const` `inline`

```cpp
virtual inline i64 fault() const
```

Defined in utlib/include/utl/types/Exception.h:57

Returns the error code.

---

### errstr

`virtual` `const` `inline` `noexcept`

```cpp
virtual inline const char * errstr() const noexcept
```

Defined in utlib/include/utl/types/Exception.h:59

Returns the name of the error code.

#### Reimplemented by

- [`IOError::errstr`](utl-IOError.md#errstr-1)
- [`CoreError::errstr`](vc64-CoreError.md#errstr-2)
- [`MediaError::errstr`](vc64-MediaError.md#errstr-3)

