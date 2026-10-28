---
title: "DatasetteInfo"
---
# DatasetteInfo

```cpp
#include <Peripherals/Datasette/DatasetteTypes.h>
```

```cpp
struct DatasetteInfo
```

Defined in Peripherals/Datasette/DatasetteTypes.h:63

Datasette current state.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`hasTape`](#hastape)  | Is a tape inserted? |
| `u8` | [`type`](#type-6)  | TAP file format. |
| `bool` | [`motor`](#motor)  | Is the motor on? |
| `bool` | [`playKey`](#playkey)  | Is the play key pressed? |
| `isize` | [`counter`](#counter)  | Progress counter. |

---

### hasTape

```cpp
bool hasTape
```

Defined in Peripherals/Datasette/DatasetteTypes.h:65

Is a tape inserted?

---

### type

```cpp
u8 type
```

Defined in Peripherals/Datasette/DatasetteTypes.h:66

TAP file format.

---

### motor

```cpp
bool motor
```

Defined in Peripherals/Datasette/DatasetteTypes.h:67

Is the motor on?

---

### playKey

```cpp
bool playKey
```

Defined in Peripherals/Datasette/DatasetteTypes.h:68

Is the play key pressed?

---

### counter

```cpp
isize counter
```

Defined in Peripherals/Datasette/DatasetteTypes.h:69

Progress counter.

