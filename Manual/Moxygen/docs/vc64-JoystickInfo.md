---
title: "JoystickInfo"
kind: struct
---
# JoystickInfo

```cpp
#include <Peripherals/Joystick/JoystickTypes.h>
```

```cpp
struct JoystickInfo
```

Defined in Peripherals/Joystick/JoystickTypes.h:89

State of a joystick.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`button`](#button)  | The fire button is pressed. |
| `int` | [`axisX`](#axisx)  | Horizontal axis (-1 left, 0 neutral, 1 right). |
| `int` | [`axisY`](#axisy)  | Vertical axis (-1 up, 0 neutral, 1 down). |

---

### button

```cpp
bool button
```

Defined in Peripherals/Joystick/JoystickTypes.h:91

The fire button is pressed.

---

### axisX

```cpp
int axisX
```

Defined in Peripherals/Joystick/JoystickTypes.h:92

Horizontal axis (-1 left, 0 neutral, 1 right).

---

### axisY

```cpp
int axisY
```

Defined in Peripherals/Joystick/JoystickTypes.h:93

Vertical axis (-1 up, 0 neutral, 1 down).

