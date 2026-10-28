---
title: "SnapshotHeader"
kind: struct
---
# SnapshotHeader

```cpp
#include <Media/Snapshot.h>
```

```cpp
struct SnapshotHeader
```

Defined in Media/Snapshot.h:43

Header of a snapshot.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `char` | [`magic`](#magic)  | Magic bytes ('V','C','6','4'). |
| `u8` | [`major`](#major)  | Major version number. |
| `u8` | [`minor`](#minor)  | Minor version number. |
| `u8` | [`subminor`](#subminor)  | Subminor version number. |
| `u8` | [`beta`](#beta)  | Beta release number. |
| `i32` | [`rawSize`](#rawsize)  | Size of this snapshot when uncompressed. |
| `u8` | [`compressor`](#compressor-1)  | Applied compression method. |
| [`Thumbnail`](vc64-Thumbnail.md#thumbnail) | [`screenshot`](#screenshot)  | Preview image. |

---

### magic

```cpp
char magic[4]
```

Defined in Media/Snapshot.h:46

Magic bytes ('V','C','6','4').

---

### major

```cpp
u8 major
```

Defined in Media/Snapshot.h:49

Major version number.

---

### minor

```cpp
u8 minor
```

Defined in Media/Snapshot.h:50

Minor version number.

---

### subminor

```cpp
u8 subminor
```

Defined in Media/Snapshot.h:51

Subminor version number.

---

### beta

```cpp
u8 beta
```

Defined in Media/Snapshot.h:52

Beta release number.

---

### rawSize

```cpp
i32 rawSize
```

Defined in Media/Snapshot.h:55

Size of this snapshot when uncompressed.

---

### compressor

```cpp
u8 compressor
```

Defined in Media/Snapshot.h:58

Applied compression method.

---

### screenshot

```cpp
Thumbnail screenshot
```

Type: [`Thumbnail`](vc64-Thumbnail.md#thumbnail)

Defined in Media/Snapshot.h:61

Preview image.

