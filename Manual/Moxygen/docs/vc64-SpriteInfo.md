---
title: "SpriteInfo"
---
# SpriteInfo

```cpp
#include <Components/VICII/VICIITypes.h>
```

```cpp
struct SpriteInfo
```

Defined in Components/VICII/VICIITypes.h:425

State of a sprite.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `u16` | [`x`](#x-3)  | Horizontal position. |
| `u8` | [`y`](#y-3)  | Vertical position. |
| `bool` | [`enabled`](#enabled-3)  | The sprite is enabled. |
| `bool` | [`expandX`](#expandx)  | Horizontal expansion. |
| `bool` | [`expandY`](#expandy)  | Vertical expansion. |
| `bool` | [`priority`](#priority)  | The sprite is drawn behind the background. |
| `bool` | [`multicolor`](#multicolor)  | Multicolor mode. |
| `bool` | [`ssCollision`](#sscollision)  | A sprite-sprite collision was detected. |
| `bool` | [`sbCollision`](#sbcollision)  | A sprite-background collision was detected. |
| `u8` | [`color`](#color)  | Sprite color. |
| `u8` | [`extraColor1`](#extracolor1)  | Extra color 1 (multicolor mode). |
| `u8` | [`extraColor2`](#extracolor2)  | Extra color 2 (multicolor mode). |

---

### x

```cpp
u16 x
```

Defined in Components/VICII/VICIITypes.h:427

Horizontal position.

---

### y

```cpp
u8 y
```

Defined in Components/VICII/VICIITypes.h:428

Vertical position.

---

### enabled

```cpp
bool enabled
```

Defined in Components/VICII/VICIITypes.h:429

The sprite is enabled.

---

### expandX

```cpp
bool expandX
```

Defined in Components/VICII/VICIITypes.h:430

Horizontal expansion.

---

### expandY

```cpp
bool expandY
```

Defined in Components/VICII/VICIITypes.h:431

Vertical expansion.

---

### priority

```cpp
bool priority
```

Defined in Components/VICII/VICIITypes.h:432

The sprite is drawn behind the background.

---

### multicolor

```cpp
bool multicolor
```

Defined in Components/VICII/VICIITypes.h:433

Multicolor mode.

---

### ssCollision

```cpp
bool ssCollision
```

Defined in Components/VICII/VICIITypes.h:434

A sprite-sprite collision was detected.

---

### sbCollision

```cpp
bool sbCollision
```

Defined in Components/VICII/VICIITypes.h:435

A sprite-background collision was detected.

---

### color

```cpp
u8 color
```

Defined in Components/VICII/VICIITypes.h:436

Sprite color.

---

### extraColor1

```cpp
u8 extraColor1
```

Defined in Components/VICII/VICIITypes.h:437

Extra color 1 (multicolor mode).

---

### extraColor2

```cpp
u8 extraColor2
```

Defined in Components/VICII/VICIITypes.h:438

Extra color 2 (multicolor mode).

