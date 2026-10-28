---
title: "IOError"
kind: struct
---
# IOError

```cpp
#include <utlib/include/utl/io/IOError.h>
```

```cpp
struct IOError
```

Defined in utlib/include/utl/io/IOError.h:21

[Exception](/reference/utl-Exception.md#exception) thrown if a file or folder cannot be accessed.

The error code (see [fault()](/reference/utl-Error.md#fault)) is one of the constants below. The message returned by [what()](/reference/utl-GenericException.md#what) names the affected file or folder.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `const char *` | [`errstr`](#errstr-1) `virtual` `const` `inline` `noexcept` `override` | Returns the name of the error code. |
|  | [`IOError`](#ioerror-1) `inline` `explicit` | Creates an error with an error code and the name of the affected file or folder. |

---

### errstr

`virtual` `const` `inline` `noexcept` `override`

```cpp
virtual inline const char * errstr() const noexcept override
```

Defined in utlib/include/utl/io/IOError.h:46

Returns the name of the error code.

#### Reimplements

- [`Error::errstr`](/reference/utl-Error.md#errstr)

---

### IOError

`inline` `explicit`

```cpp
inline explicit IOError(long fault, const std::string & msg = "")
```

Defined in utlib/include/utl/io/IOError.h:78

Creates an error with an error code and the name of the affected file or folder.

## Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `long` | [`DIR_NOT_FOUND`](#dir_not_found) `static` `constexpr` | The folder does not exist. |
| `long` | [`DIR_ACCESS_DENIED`](#dir_access_denied) `static` `constexpr` | Access to the folder was denied. |
| `long` | [`DIR_CANT_CREATE`](#dir_cant_create) `static` `constexpr` | The folder could not be created. |
| `long` | [`DIR_CANT_DELETE`](#dir_cant_delete) `static` `constexpr` | The folder could not be deleted. |
| `long` | [`DIR_NOT_EMPTY`](#dir_not_empty) `static` `constexpr` | The folder is not empty. |
| `long` | [`FILE_NOT_FOUND`](#file_not_found) `static` `constexpr` | The file does not exist. |
| `long` | [`FILE_EXISTS`](#file_exists) `static` `constexpr` | The file exists already. |
| `long` | [`FILE_IS_DIRECTORY`](#file_is_directory) `static` `constexpr` | The path refers to a folder, not a file. |
| `long` | [`FILE_ACCESS_DENIED`](#file_access_denied) `static` `constexpr` | Access to the file was denied. |
| `long` | [`FILE_TYPE_MISMATCH`](#file_type_mismatch) `static` `constexpr` | The file has the wrong type. |
| `long` | [`FILE_TYPE_UNSUPPORTED`](#file_type_unsupported) `static` `constexpr` | The file type is not supported. |
| `long` | [`FILE_CANT_READ`](#file_cant_read) `static` `constexpr` | The file could not be read. |
| `long` | [`FILE_CANT_WRITE`](#file_cant_write) `static` `constexpr` | The file could not be written. |
| `long` | [`FILE_CANT_CREATE`](#file_cant_create) `static` `constexpr` | The file could not be created. |
| `long` | [`FILE_CANT_DELETE`](#file_cant_delete) `static` `constexpr` | The file could not be deleted. |
| `long` | [`FILE_INVALID_DATA`](#file_invalid_data) `static` `constexpr` | The file contains invalid data. |
| `long` | [`FILE_IN_USE`](#file_in_use) `static` `constexpr` | The file is in use. |
| `long` | [`JSON_ERROR`](#json_error) `static` `constexpr` | A JSON document could not be processed. |
| `long` | [`ZLIB_ERROR`](#zlib_error) `static` `constexpr` | Compression or decompression failed. |

---

### DIR_NOT_FOUND

`static` `constexpr`

```cpp
long DIR_NOT_FOUND = 0
```

Defined in utlib/include/utl/io/IOError.h:23

The folder does not exist.

---

### DIR_ACCESS_DENIED

`static` `constexpr`

```cpp
long DIR_ACCESS_DENIED = 1
```

Defined in utlib/include/utl/io/IOError.h:24

Access to the folder was denied.

---

### DIR_CANT_CREATE

`static` `constexpr`

```cpp
long DIR_CANT_CREATE = 2
```

Defined in utlib/include/utl/io/IOError.h:25

The folder could not be created.

---

### DIR_CANT_DELETE

`static` `constexpr`

```cpp
long DIR_CANT_DELETE = 3
```

Defined in utlib/include/utl/io/IOError.h:26

The folder could not be deleted.

---

### DIR_NOT_EMPTY

`static` `constexpr`

```cpp
long DIR_NOT_EMPTY = 4
```

Defined in utlib/include/utl/io/IOError.h:27

The folder is not empty.

---

### FILE_NOT_FOUND

`static` `constexpr`

```cpp
long FILE_NOT_FOUND = 10
```

Defined in utlib/include/utl/io/IOError.h:29

The file does not exist.

---

### FILE_EXISTS

`static` `constexpr`

```cpp
long FILE_EXISTS = 11
```

Defined in utlib/include/utl/io/IOError.h:30

The file exists already.

---

### FILE_IS_DIRECTORY

`static` `constexpr`

```cpp
long FILE_IS_DIRECTORY = 12
```

Defined in utlib/include/utl/io/IOError.h:31

The path refers to a folder, not a file.

---

### FILE_ACCESS_DENIED

`static` `constexpr`

```cpp
long FILE_ACCESS_DENIED = 13
```

Defined in utlib/include/utl/io/IOError.h:32

Access to the file was denied.

---

### FILE_TYPE_MISMATCH

`static` `constexpr`

```cpp
long FILE_TYPE_MISMATCH = 14
```

Defined in utlib/include/utl/io/IOError.h:33

The file has the wrong type.

---

### FILE_TYPE_UNSUPPORTED

`static` `constexpr`

```cpp
long FILE_TYPE_UNSUPPORTED = 15
```

Defined in utlib/include/utl/io/IOError.h:34

The file type is not supported.

---

### FILE_CANT_READ

`static` `constexpr`

```cpp
long FILE_CANT_READ = 16
```

Defined in utlib/include/utl/io/IOError.h:35

The file could not be read.

---

### FILE_CANT_WRITE

`static` `constexpr`

```cpp
long FILE_CANT_WRITE = 17
```

Defined in utlib/include/utl/io/IOError.h:36

The file could not be written.

---

### FILE_CANT_CREATE

`static` `constexpr`

```cpp
long FILE_CANT_CREATE = 18
```

Defined in utlib/include/utl/io/IOError.h:37

The file could not be created.

---

### FILE_CANT_DELETE

`static` `constexpr`

```cpp
long FILE_CANT_DELETE = 19
```

Defined in utlib/include/utl/io/IOError.h:38

The file could not be deleted.

---

### FILE_INVALID_DATA

`static` `constexpr`

```cpp
long FILE_INVALID_DATA = 20
```

Defined in utlib/include/utl/io/IOError.h:39

The file contains invalid data.

---

### FILE_IN_USE

`static` `constexpr`

```cpp
long FILE_IN_USE = 21
```

Defined in utlib/include/utl/io/IOError.h:40

The file is in use.

---

### JSON_ERROR

`static` `constexpr`

```cpp
long JSON_ERROR = 30
```

Defined in utlib/include/utl/io/IOError.h:42

A JSON document could not be processed.

---

### ZLIB_ERROR

`static` `constexpr`

```cpp
long ZLIB_ERROR = 31
```

Defined in utlib/include/utl/io/IOError.h:43

Compression or decompression failed.

