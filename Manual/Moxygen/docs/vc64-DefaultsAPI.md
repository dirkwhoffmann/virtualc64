---
title: "DefaultsAPI"
---
# DefaultsAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class DefaultsAPI
```

Defined in VirtualC64.h:1546

The user's defaults storage

The defaults storage manages all configuration settings that persist across multiple application launches. It provides the following functionality:

* **Loading and saving the storage data**
You can persist the user's defaults storage in a file, a stream, or a string stream.
* **Reading and writing key-value pairs**
The return value is read from the user's defaults storage for registered keys. For unknown keys, an exception is thrown.
* **Registerung fallback values**
The fallback value is used for registered keys with no custom value set.

  > [!NOTE]
  > Setting a fallback value for an unknown key is permitted. In this case, a new key is registered together with the provided default value. The GUI utilizes this feature to register additional keys, such as keys storing shader-relevant parameters that are irrelevant to the emulation core.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`load`](#load)  | Loads a storage file from disk. |
| `void` | [`load`](#load-1)  | Loads a storage file from a stream. |
| `void` | [`load`](#load-2)  | Loads a storage file from a string stream. |
| `void` | [`save`](#save)  | Saves a storage file to disk. |
| `void` | [`save`](#save-1)  | Saves a storage file to stream. |
| `void` | [`save`](#save-2)  | Saves a storage file to a string stream. |
| `string` | [`getRaw`](#getraw) `const` | Queries a key-value pair. |
| `i64` | [`get`](#get) `const` | Queries a key-value pair. |
| `i64` | [`get`](#get-1) `const` | Queries a key-value pair. |
| `string` | [`getFallbackRaw`](#getfallbackraw) `const` | Queries a fallback key-value pair. |
| `i64` | [`getFallback`](#getfallback) `const` | Queries a fallback key-value pair. |
| `i64` | [`getFallback`](#getfallback-1) `const` | Queries a fallback key-value pair. |
| `void` | [`set`](#set)  | Writes a key-value pair into the user storage. |
| `void` | [`set`](#set-1)  | Writes a key-value pair into the user storage. |
| `void` | [`set`](#set-2)  | Writes multiple key-value pairs into the user storage. |
| `void` | [`set`](#set-3)  | Writes a key-value pair into the user storage. |
| `void` | [`set`](#set-4)  | Writes multiple key-value pairs into the user storage. |
| `void` | [`setFallback`](#setfallback)  | Writes a key-value pair into the fallback storage. |
| `void` | [`setFallback`](#setfallback-1)  | Writes a key-value pair into the fallback storage. |
| `void` | [`setFallback`](#setfallback-2)  | Writes multiple key-value pairs into the fallback storage. |
| `void` | [`setFallback`](#setfallback-3)  | Writes a key-value pair into the fallback storage. |
| `void` | [`setFallback`](#setfallback-4)  | Writes multiple key-value pairs into the fallback storage. |
| `void` | [`remove`](#remove)  | Deletes all key-value pairs. |
| `void` | [`remove`](#remove-1)  | Deletes a key-value pair. |
| `void` | [`remove`](#remove-2)  | Deletes a key-value pair. |
| `void` | [`remove`](#remove-3)  | Deletes multiple key-value pairs. |

---

### load

```cpp
void load(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1564

Loads a storage file from disk.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be read or does not contain valid data. |

---

### load

```cpp
void load(std::ifstream & stream)
```

Defined in VirtualC64.h:1570

Loads a storage file from a stream.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be read or does not contain valid data. |

---

### load

```cpp
void load(std::stringstream & stream)
```

Defined in VirtualC64.h:1576

Loads a storage file from a string stream.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be read or does not contain valid data. |

---

### save

```cpp
void save(const std::filesystem::path & path)
```

Defined in VirtualC64.h:1582

Saves a storage file to disk.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be written. |

---

### save

```cpp
void save(std::ofstream & stream)
```

Defined in VirtualC64.h:1588

Saves a storage file to stream.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be written. |

---

### save

```cpp
void save(std::stringstream & stream)
```

Defined in VirtualC64.h:1594

Saves a storage file to a string stream.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be written. |

---

### getRaw

`const`

```cpp
string getRaw(const string & key) const
```

Defined in VirtualC64.h:1605

Queries a key-value pair.

#### Returns
The value as a string.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const string &` | The key. |

---

### get

`const`

```cpp
i64 get(const string & key) const
```

Defined in VirtualC64.h:1611

Queries a key-value pair.

#### Returns
The value as an integer. 0 if the value cannot not be parsed.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const string &` | The key. |

---

### get

`const`

```cpp
i64 get(Opt opt, isize nr = 0) const
```

Defined in VirtualC64.h:1618

Queries a key-value pair.

#### Returns
The value as an integer.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | A config option whose name is used as the prefix of the key. |
| `nr` | `isize` | Optional number that is appened to the key as suffix. |

---

### getFallbackRaw

`const`

```cpp
string getFallbackRaw(const string & key) const
```

Defined in VirtualC64.h:1624

Queries a fallback key-value pair.

#### Returns
The value as a string.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const string &` | The key. |

---

### getFallback

`const`

```cpp
i64 getFallback(const string & key) const
```

Defined in VirtualC64.h:1630

Queries a fallback key-value pair.

#### Returns
The value as an integer. 0 if the value cannot not be parsed.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const string &` | The key. |

---

### getFallback

`const`

```cpp
i64 getFallback(Opt opt, isize nr = 0) const
```

Defined in VirtualC64.h:1637

Queries a fallback key-value pair.

#### Returns
The value as an integer.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | A config option whose name is used as the key. |
| `nr` | `isize` | Optional number that is appened to the key as suffix. |

---

### set

```cpp
void set(const string & key, const string & value)
```

Defined in VirtualC64.h:1648

Writes a key-value pair into the user storage.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const string &` | The key, given as a string. |
| `value` | `const string &` | The value, given as a string. |

---

### set

```cpp
void set(Opt opt, const string & value)
```

Defined in VirtualC64.h:1654

Writes a key-value pair into the user storage.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option's name forms the prefix of the keys. |
| `value` | `const string &` | The value, given as a string. |

---

### set

```cpp
void set(Opt opt, const string & value, std::vector< isize > objids)
```

Defined in VirtualC64.h:1661

Writes multiple key-value pairs into the user storage.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option's name forms the prefix of the keys. |
| `value` | `const string &` | The value for all pairs, given as a string. |
| `objids` | `std::vector< isize >` | The keys are parameterized by adding the vector values as suffixes. |

---

### set

```cpp
void set(Opt opt, i64 value)
```

Defined in VirtualC64.h:1667

Writes a key-value pair into the user storage.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option's name forms the prefix of the keys. |
| `value` | `i64` | The value, given as an integer. |

---

### set

```cpp
void set(Opt opt, i64 value, std::vector< isize > objids)
```

Defined in VirtualC64.h:1674

Writes multiple key-value pairs into the user storage.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option's name forms the prefix of the keys. |
| `value` | `i64` | The value for all pairs, given as an integer. |
| `objids` | `std::vector< isize >` | The keys are parameterized by adding the vector values as suffixes. |

---

### setFallback

```cpp
void setFallback(const string & key, const string & value)
```

Defined in VirtualC64.h:1680

Writes a key-value pair into the fallback storage.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const string &` | The key, given as a string. |
| `value` | `const string &` | The value, given as a string. |

---

### setFallback

```cpp
void setFallback(Opt opt, const string & value)
```

Defined in VirtualC64.h:1686

Writes a key-value pair into the fallback storage.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option's name forms the prefix of the keys. |
| `value` | `const string &` | The value, given as an integer. |

---

### setFallback

```cpp
void setFallback(Opt opt, const string & value, std::vector< isize > objids)
```

Defined in VirtualC64.h:1693

Writes multiple key-value pairs into the fallback storage.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option's name forms the prefix of the keys. |
| `value` | `const string &` | The shared value for all pairs, given as a string. |
| `objids` | `std::vector< isize >` | The keys are parameterized by adding the vector values as suffixes. |

---

### setFallback

```cpp
void setFallback(Opt opt, i64 value)
```

Defined in VirtualC64.h:1699

Writes a key-value pair into the fallback storage.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option's name forms the prefix of the keys. |
| `value` | `i64` | The value, given as an integer. |

---

### setFallback

```cpp
void setFallback(Opt opt, i64 value, std::vector< isize > objids)
```

Defined in VirtualC64.h:1706

Writes multiple key-value pairs into the fallback storage.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option's name forms the prefix of the keys. |
| `value` | `i64` | The shared value for all pairs, given as an integer. |
| `objids` | `std::vector< isize >` | The keys are parameterized by adding the vector values as suffixes. |

---

### remove

```cpp
void remove()
```

Defined in VirtualC64.h:1715

Deletes all key-value pairs.

---

### remove

```cpp
void remove(const string & key)
```

Defined in VirtualC64.h:1720

Deletes a key-value pair.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const string &` | The key of the key-value pair. |

---

### remove

```cpp
void remove(Opt opt)
```

Defined in VirtualC64.h:1725

Deletes a key-value pair.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option's name forms the key. |

---

### remove

```cpp
void remove(Opt opt, std::vector< isize > objids)
```

Defined in VirtualC64.h:1731

Deletes multiple key-value pairs.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option's name forms the prefix of the keys. |
| `objids` | `std::vector< isize >` | The keys are parameterized by adding the vector values as suffixes. |

