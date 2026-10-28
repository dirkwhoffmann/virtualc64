---
title: "C64Config"
kind: struct
---
# C64Config

```cpp
#include <Components/C64Types.h>
```

```cpp
struct C64Config
```

Defined in Components/C64Types.h:302

Configuration of the C64 component.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`warpBoot`](#warpboot)  | After a reset, the emulator runs in warp mode for this amout of seconds. |
| [`Warp`](/reference/enum-Warp.md) | [`warpMode`](#warpmode)  | [Warp](/reference/enum-Warp.md) mode. |
| `isize` | [`speedBoost`](#speedboost)  | Emulator speed in percent (100 is native speed). |
| `bool` | [`vsync`](#vsync)  | Vertical Synchronization. |
| `isize` | [`runAhead`](#runahead)  | Number of run-ahead frames (0 = run-ahead is disabled). |

---

### warpBoot

```cpp
isize warpBoot
```

Defined in Components/C64Types.h:305

After a reset, the emulator runs in warp mode for this amout of seconds.

---

### warpMode

```cpp
Warp warpMode
```

Type: [`Warp`](/reference/enum-Warp.md)

Defined in Components/C64Types.h:308

[Warp](/reference/enum-Warp.md) mode.

---

### speedBoost

```cpp
isize speedBoost
```

Defined in Components/C64Types.h:311

Emulator speed in percent (100 is native speed).

---

### vsync

```cpp
bool vsync
```

Defined in Components/C64Types.h:314

Vertical Synchronization.

---

### runAhead

```cpp
isize runAhead
```

Defined in Components/C64Types.h:317

Number of run-ahead frames (0 = run-ahead is disabled).

