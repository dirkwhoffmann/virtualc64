---
title: "GenericException"
---
# GenericException

```cpp
#include <utlib/include/utl/types/Exception.h>
```

```cpp
template<typename T>
struct GenericException
```

Defined in utlib/include/utl/types/Exception.h:26

An exception that carries a payload and a message.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `T` | [`payload`](#payload)  | Exception-specific payload. |
| `std::string` | [`msg`](#msg)  | Message returned by [what()](#what). |

---

### payload

```cpp
T payload
```

Defined in utlib/include/utl/types/Exception.h:28

Exception-specific payload.

---

### msg

```cpp
std::string msg
```

Defined in utlib/include/utl/types/Exception.h:29

Message returned by [what()](#what).

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `const char *` | [`what`](#what) `const` `inline` `noexcept` `override` | Returns the message. |

---

### what

`const` `inline` `noexcept` `override`

```cpp
inline const char * what() const noexcept override
```

Defined in utlib/include/utl/types/Exception.h:35

Returns the message.

