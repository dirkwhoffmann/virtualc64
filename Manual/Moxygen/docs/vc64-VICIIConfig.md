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
| [`VICIIRev`](grp_vicii.md#viciirev) | [`revision`](#revision-2)  | Chip revision. |
| [`VICIIRev`](grp_vicii.md#viciirev) | [`awaiting`](#awaiting)  | Chip revision that takes effect on the next reset. |
| `bool` | [`powerSave`](#powersave-1)  | Take fast paths. |
| `bool` | [`grayDotBug`](#graydotbug)  | Emulate the gray-dot bug. |
| [`GlueLogic`](grp_vicii.md#gluelogic) | [`glueLogic`](#gluelogic-1)  | Glue logic type. |
| `bool` | [`hideSprites`](#hidesprites)  | Sprites. |
| `bool` | [`checkSSCollisions`](#checksscollisions)  | Detect sprite-sprite collisions. |
| `bool` | [`checkSBCollisions`](#checksbcollisions)  | Detect sprite-background collisions. |

---

### revision

```cpp
VICIIRev revision
```

Type: [`VICIIRev`](grp_vicii.md#viciirev)

Defined in Components/VICII/VICIITypes.h:337

Chip revision.

---

### awaiting

```cpp
VICIIRev awaiting
```

Type: [`VICIIRev`](grp_vicii.md#viciirev)

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

Type: [`GlueLogic`](grp_vicii.md#gluelogic)

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

