---
title: "ControlPortAPI"
kind: class
---
# ControlPortAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class ControlPortAPI
```

Defined in VirtualC64.h:1153

Control Port [API](vc64-API.md#api)

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`JoystickAPI`](vc64-JoystickAPI.md#joystickapi) | [`joystick`](#joystick)  | Custom [API](vc64-API.md#api) of the joystick connected to this port. |
| [`MouseAPI`](vc64-MouseAPI.md#mouseapi) | [`mouse`](#mouse)  | Custom [API](vc64-API.md#api) of the mouse connected to this port. |
| [`PaddleAPI`](vc64-PaddleAPI.md#paddleapi) | [`paddle`](#paddle)  | Custom [API](vc64-API.md#api) of the paddle connected to this port. |

---

### joystick

```cpp
JoystickAPI joystick
```

Type: [`JoystickAPI`](vc64-JoystickAPI.md#joystickapi)

Defined in VirtualC64.h:1164

Custom [API](vc64-API.md#api) of the joystick connected to this port.

Joystick connected to this port

---

### mouse

```cpp
MouseAPI mouse
```

Type: [`MouseAPI`](vc64-MouseAPI.md#mouseapi)

Defined in VirtualC64.h:1169

Custom [API](vc64-API.md#api) of the mouse connected to this port.

Mouse connected to this port

---

### paddle

```cpp
PaddleAPI paddle
```

Type: [`PaddleAPI`](vc64-PaddleAPI.md#paddleapi)

Defined in VirtualC64.h:1174

Custom [API](vc64-API.md#api) of the paddle connected to this port.

Paddle connected to this port

