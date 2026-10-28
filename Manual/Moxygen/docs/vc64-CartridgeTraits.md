---
title: "CartridgeTraits"
---
# CartridgeTraits

```cpp
#include <Media/Cartridges/CartridgeTypes.h>
```

```cpp
struct CartridgeTraits
```

Defined in Media/Cartridges/CartridgeTypes.h:264

Properties of a cartridge type.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`CartridgeType`](enum-CartridgeType.md) | [`type`](#type-3)  | Cartridge type. |
| `const char *` | [`title`](#title)  | Title of the cartridge. |
| `isize` | [`memory`](#memory)  | Memory size (in KB). |
| `bool` | [`battery`](#battery)  | The cartridge has battery-buffered memory. |
| `isize` | [`buttons`](#buttons)  | Number of buttons. |
| `const char *` | [`button1`](#button1)  | Label of button 1. |
| `const char *` | [`button2`](#button2)  | Label of button 2. |
| `isize` | [`switches`](#switches)  | Number of switches. |
| `const char *` | [`switchLeft`](#switchleft)  | Label of the left switch position. |
| `const char *` | [`switchNeutral`](#switchneutral)  | Label of the neutral switch position. |
| `const char *` | [`switchRight`](#switchright)  | Label of the right switch position. |
| `isize` | [`leds`](#leds)  | Number of LEDs. |
| `bool` | [`needsExecution`](#needsexecution)  | The cartridge needs to be executed every cycle. |

---

### type

```cpp
CartridgeType type
```

Type: [`CartridgeType`](enum-CartridgeType.md)

Defined in Media/Cartridges/CartridgeTypes.h:266

Cartridge type.

---

### title

```cpp
const char * title
```

Defined in Media/Cartridges/CartridgeTypes.h:267

Title of the cartridge.

---

### memory

```cpp
isize memory
```

Defined in Media/Cartridges/CartridgeTypes.h:269

Memory size (in KB).

---

### battery

```cpp
bool battery
```

Defined in Media/Cartridges/CartridgeTypes.h:270

The cartridge has battery-buffered memory.

---

### buttons

```cpp
isize buttons
```

Defined in Media/Cartridges/CartridgeTypes.h:272

Number of buttons.

---

### button1

```cpp
const char * button1
```

Defined in Media/Cartridges/CartridgeTypes.h:273

Label of button 1.

---

### button2

```cpp
const char * button2
```

Defined in Media/Cartridges/CartridgeTypes.h:274

Label of button 2.

---

### switches

```cpp
isize switches
```

Defined in Media/Cartridges/CartridgeTypes.h:276

Number of switches.

---

### switchLeft

```cpp
const char * switchLeft
```

Defined in Media/Cartridges/CartridgeTypes.h:277

Label of the left switch position.

---

### switchNeutral

```cpp
const char * switchNeutral
```

Defined in Media/Cartridges/CartridgeTypes.h:278

Label of the neutral switch position.

---

### switchRight

```cpp
const char * switchRight
```

Defined in Media/Cartridges/CartridgeTypes.h:279

Label of the right switch position.

---

### leds

```cpp
isize leds
```

Defined in Media/Cartridges/CartridgeTypes.h:281

Number of LEDs.

---

### needsExecution

```cpp
bool needsExecution
```

Defined in Media/Cartridges/CartridgeTypes.h:283

The cartridge needs to be executed every cycle.

