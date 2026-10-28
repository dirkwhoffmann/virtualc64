---
title: "VICIIConfig"
---
# VICIIConfig

```cpp
#include <Components/VICII/VICIITypes.h>
```

```cpp
struct VICIIConfig
```

Defined in Components/VICII/VICIITypes.h:334

Configuration of the VICII chip.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`VICIIRev`](enum-VICIIRev.md) | [`revision`](#revision-3)  | Chip revision. |
| [`VICIIRev`](enum-VICIIRev.md) | [`awaiting`](#awaiting)  | Chip revision that takes effect on the next reset. |
| `bool` | [`powerSave`](#powersave-2)  | Take fast paths. |
| `bool` | [`grayDotBug`](#graydotbug)  | Emulate the gray-dot bug. |
| [`GlueLogic`](enum-GlueLogic.md) | [`glueLogic`](#gluelogic-1)  | Glue logic type. |
| `bool` | [`hideSprites`](#hidesprites)  | Sprites. |
| `bool` | [`checkSSCollisions`](#checksscollisions)  | Detect sprite-sprite collisions. |
| `bool` | [`checkSBCollisions`](#checksbcollisions)  | Detect sprite-background collisions. |

---

### revision

```cpp
VICIIRev revision
```

Type: [`VICIIRev`](enum-VICIIRev.md)

Defined in Components/VICII/VICIITypes.h:337

Chip revision.

---

### awaiting

```cpp
VICIIRev awaiting
```

Type: [`VICIIRev`](enum-VICIIRev.md)

Defined in Components/VICII/VICIITypes.h:338

Chip revision that takes effect on the next reset.

---

### powerSave

```cpp
bool powerSave
```

Defined in Components/VICII/VICIITypes.h:339

Take fast paths.

---

### grayDotBug

```cpp
bool grayDotBug
```

Defined in Components/VICII/VICIITypes.h:340

Emulate the gray-dot bug.

---

### glueLogic

```cpp
GlueLogic glueLogic
```

Type: [`GlueLogic`](enum-GlueLogic.md)

Defined in Components/VICII/VICIITypes.h:341

Glue logic type.

---

### hideSprites

```cpp
bool hideSprites
```

Defined in Components/VICII/VICIITypes.h:344

Sprites.

---

### checkSSCollisions

```cpp
bool checkSSCollisions
```

Defined in Components/VICII/VICIITypes.h:347

Detect sprite-sprite collisions.

---

### checkSBCollisions

```cpp
bool checkSBCollisions
```

Defined in Components/VICII/VICIITypes.h:348

Detect sprite-background collisions.

