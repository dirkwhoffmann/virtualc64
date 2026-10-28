---
title: "Thumbnail"
kind: struct
---
# Thumbnail

```cpp
#include <Media/Snapshot.h>
```

```cpp
struct Thumbnail
```

Defined in Media/Snapshot.h:24

Preview image of a snapshot.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `i32` | [`width`](#width)  | Image size. |
| `u32` | [`screen`](#screen)  | Raw texture data. |
| `time_t` | [`timestamp`](#timestamp-1)  | Creation date and time. |

---

### width

```cpp
i32 width
```

Defined in Media/Snapshot.h:27

Image size.

---

### screen

```cpp
u32 screen[Tex::height *Tex::width]
```

Defined in Media/Snapshot.h:30

Raw texture data.

---

### timestamp

```cpp
time_t timestamp
```

Defined in Media/Snapshot.h:33

Creation date and time.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`take`](#take)  | Takes a screenshot from the current texture. |

---

### take

```cpp
void take(const C64 & c64, isize dx = 1, isize dy = 1)
```

Defined in Media/Snapshot.h:39

Takes a screenshot from the current texture.

## Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| [`Thumbnail`](#thumbnail) * | [`makeWithC64`](#makewithc64) `static` | Factory methods. |

---

### makeWithC64

`static`

```cpp
static Thumbnail * makeWithC64(const C64 & c64, isize dx = 1, isize dy = 1)
```

Defined in Media/Snapshot.h:36

Factory methods.

