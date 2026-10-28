---
title: "RomInfo"
kind: struct
---
# RomInfo

```cpp
#include <Components/C64Types.h>
```

```cpp
struct RomInfo
```

Defined in Components/C64Types.h:322

Description of an installed ROM.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `u32` | [`crc32`](#crc32-1)  | CRC32 checksum. |
| `u64` | [`fnv64`](#fnv64-1)  | FNV-64 checksum. |
| `const char *` | [`title`](#title-1)  | Title of the ROM. |
| `const char *` | [`subtitle`](#subtitle)  | Subtitle of the ROM. |
| `const char *` | [`revision`](#revision-1)  | Revision of the ROM. |
| `bool` | [`isCommodoreRom`](#iscommodorerom)  | The ROM is an original Commodore ROM. |
| `bool` | [`isPatchedRom`](#ispatchedrom)  | The ROM is a patched variant. |
| `bool` | [`isMega65Rom`](#ismega65rom)  | The ROM is an OpenROM of the MEGA65 project. |

---

### crc32

```cpp
u32 crc32
```

Defined in Components/C64Types.h:324

CRC32 checksum.

---

### fnv64

```cpp
u64 fnv64
```

Defined in Components/C64Types.h:325

FNV-64 checksum.

---

### title

```cpp
const char * title
```

Defined in Components/C64Types.h:327

Title of the ROM.

---

### subtitle

```cpp
const char * subtitle
```

Defined in Components/C64Types.h:328

Subtitle of the ROM.

---

### revision

```cpp
const char * revision
```

Defined in Components/C64Types.h:329

Revision of the ROM.

---

### isCommodoreRom

```cpp
bool isCommodoreRom
```

Defined in Components/C64Types.h:331

The ROM is an original Commodore ROM.

---

### isPatchedRom

```cpp
bool isPatchedRom
```

Defined in Components/C64Types.h:332

The ROM is a patched variant.

---

### isMega65Rom

```cpp
bool isMega65Rom
```

Defined in Components/C64Types.h:333

The ROM is an OpenROM of the MEGA65 project.

