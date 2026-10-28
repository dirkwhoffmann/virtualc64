---
title: "MediaError"
---
# MediaError

```cpp
#include <Media/MediaError.h>
```

```cpp
struct MediaError
```

Defined in Media/MediaError.h:24

Exception thrown if a media file is invalid or not supported.

Media files are ROMs, snapshots, cartridges, disk images, and similar files. The error code (see [fault()](utl-Error.md#fault)) is one of the constants below.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `const char *` | [`errstr`](#errstr-3) `virtual` `const` `inline` `noexcept` `override` | Returns the name of the error code. |
|  | [`MediaError`](#mediaerror-1) `explicit` | Creates an error with an error code and a descriptive string. |
|  | [`MediaError`](#mediaerror-2) `inline` `explicit` | Creates an error with an error code and a descriptive string. |
|  | [`MediaError`](#mediaerror-3) `inline` `explicit` | Creates an error with an error code and a file path. |
|  | [`MediaError`](#mediaerror-4) `inline` `explicit` | Creates an error with an error code and a number. |
|  | [`MediaError`](#mediaerror-5) `inline` `explicit` | Creates an error with an error code. |

---

### errstr

`virtual` `const` `inline` `noexcept` `override`

```cpp
virtual inline const char * errstr() const noexcept override
```

Defined in Media/MediaError.h:44

Returns the name of the error code.

#### Reimplements

- [`Error::errstr`](utl-Error.md#errstr)

---

### MediaError

`explicit`

```cpp
explicit MediaError(long fault, const string & s)
```

Defined in Media/MediaError.h:66

Creates an error with an error code and a descriptive string.

---

### MediaError

`inline` `explicit`

```cpp
inline explicit MediaError(long fault, const char * s)
```

Defined in Media/MediaError.h:68

Creates an error with an error code and a descriptive string.

---

### MediaError

`inline` `explicit`

```cpp
inline explicit MediaError(long fault, const fs::path & p)
```

Defined in Media/MediaError.h:70

Creates an error with an error code and a file path.

---

### MediaError

`inline` `explicit`

```cpp
inline explicit MediaError(long fault, std::integral auto v)
```

Defined in Media/MediaError.h:72

Creates an error with an error code and a number.

---

### MediaError

`inline` `explicit`

```cpp
inline explicit MediaError(long fault)
```

Defined in Media/MediaError.h:74

Creates an error with an error code.

## Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `long` | [`OK`](#ok-1) `static` `constexpr` | No error. |
| `long` | [`UNKNOWN`](#unknown-1) `static` `constexpr` | Unclassified. |
| `long` | [`SNAP_TOO_OLD`](#snap_too_old) `static` `constexpr` | [Snapshot](vc64-Snapshot.md#snapshot) was created with an older version. |
| `long` | [`SNAP_TOO_NEW`](#snap_too_new) `static` `constexpr` | [Snapshot](vc64-Snapshot.md#snapshot) was created with a later version. |
| `long` | [`SNAP_IS_BETA`](#snap_is_beta) `static` `constexpr` | [Snapshot](vc64-Snapshot.md#snapshot) was created with a beta release. |
| `long` | [`SNAP_CORRUPTED`](#snap_corrupted) `static` `constexpr` | [Snapshot](vc64-Snapshot.md#snapshot) data is corrupted. |
| `long` | [`CRT_NO_CARTRIDGE`](#crt_no_cartridge) `static` `constexpr` | No cartridge attached. |
| `long` | [`CRT_UNKNOWN`](#crt_unknown) `static` `constexpr` | Unknown cartridge type. |
| `long` | [`CRT_UNSUPPORTED`](#crt_unsupported) `static` `constexpr` | Unsupported cartridge type. |
| `long` | [`CRT_TOO_MANY_PACKETS`](#crt_too_many_packets) `static` `constexpr` | CRT file contains too many Rom packets. |
| `long` | [`CRT_CORRUPTED_PACKET`](#crt_corrupted_packet) `static` `constexpr` | CRT file contains a corrupted Rom package. |

---

### OK

`static` `constexpr`

```cpp
long OK =  0
```

Defined in Media/MediaError.h:26

No error.

---

### UNKNOWN

`static` `constexpr`

```cpp
long UNKNOWN =  1
```

Defined in Media/MediaError.h:27

Unclassified.

---

### SNAP_TOO_OLD

`static` `constexpr`

```cpp
long SNAP_TOO_OLD = 10
```

Defined in Media/MediaError.h:30

[Snapshot](vc64-Snapshot.md#snapshot) was created with an older version.

---

### SNAP_TOO_NEW

`static` `constexpr`

```cpp
long SNAP_TOO_NEW = 11
```

Defined in Media/MediaError.h:31

[Snapshot](vc64-Snapshot.md#snapshot) was created with a later version.

---

### SNAP_IS_BETA

`static` `constexpr`

```cpp
long SNAP_IS_BETA = 12
```

Defined in Media/MediaError.h:32

[Snapshot](vc64-Snapshot.md#snapshot) was created with a beta release.

---

### SNAP_CORRUPTED

`static` `constexpr`

```cpp
long SNAP_CORRUPTED = 13
```

Defined in Media/MediaError.h:33

[Snapshot](vc64-Snapshot.md#snapshot) data is corrupted.

---

### CRT_NO_CARTRIDGE

`static` `constexpr`

```cpp
long CRT_NO_CARTRIDGE = 20
```

Defined in Media/MediaError.h:36

No cartridge attached.

---

### CRT_UNKNOWN

`static` `constexpr`

```cpp
long CRT_UNKNOWN = 21
```

Defined in Media/MediaError.h:37

Unknown cartridge type.

---

### CRT_UNSUPPORTED

`static` `constexpr`

```cpp
long CRT_UNSUPPORTED = 22
```

Defined in Media/MediaError.h:38

Unsupported cartridge type.

---

### CRT_TOO_MANY_PACKETS

`static` `constexpr`

```cpp
long CRT_TOO_MANY_PACKETS = 23
```

Defined in Media/MediaError.h:39

CRT file contains too many Rom packets.

---

### CRT_CORRUPTED_PACKET

`static` `constexpr`

```cpp
long CRT_CORRUPTED_PACKET = 24
```

Defined in Media/MediaError.h:40

CRT file contains a corrupted Rom package.

