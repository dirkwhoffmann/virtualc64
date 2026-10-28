---
title: "Control ports"
---
# Control ports

The control ports and the joysticks, mice, and paddles connected to them.

## Classes

| Name | Description |
|------|-------------|
| [`ControlPortAPI`](vc64-ControlPortAPI.md#controlportapi) | Control Port [API](vc64-API.md#api) |
| [`JoystickAPI`](vc64-JoystickAPI.md#joystickapi) | Joystick Public [API](vc64-API.md#api) |
| [`MouseAPI`](vc64-MouseAPI.md#mouseapi) | Mouse Public [API](vc64-API.md#api) |
| [`PaddleAPI`](vc64-PaddleAPI.md#paddleapi) | Paddle Public [API](vc64-API.md#api) |
| [`GamePadCmd`](vc64-GamePadCmd.md#gamepadcmd) | Payload of joystick commands. |
| [`JoystickConfig`](vc64-JoystickConfig.md#joystickconfig) | Configuration of a joystick. |
| [`JoystickInfo`](vc64-JoystickInfo.md#joystickinfo) | State of a joystick. |
| [`MouseConfig`](vc64-MouseConfig.md#mouseconfig) | Configuration of a mouse. |
| [`PaddleConfig`](vc64-PaddleConfig.md#paddleconfig) | Configuration of a paddle. |
| [`PaddleInfo`](vc64-PaddleInfo.md#paddleinfo) | State of a paddle (currently carries no data). |

## Enumerations

| Name | Description |
|------|-------------|
| [`ControlPortDevice`](#controlportdevice)  | Control port device. |
| [`GamePadAction`](#gamepadaction)  | Gamepad / Joystick event. |
| [`MouseModel`](#mousemodel)  | Mouse model. |
| [`PaddleOrientation`](#paddleorientation)  | Value mapping scheme. |

---

### ControlPortDevice

```cpp
enum ControlPortDevice
```

Defined in Ports/ControlPortTypes.h:35

Control port device.

| Value | Description |
|-------|-------------|
| `NONE` | No device. |
| `MOUSE` | Mouse. |
| `JOYSTICK` | Joystick. |

---

### GamePadAction

```cpp
enum GamePadAction
```

Defined in Peripherals/Joystick/JoystickTypes.h:25

Gamepad / Joystick event.

| Value | Description |
|-------|-------------|
| `PULL_UP` | Pull the joystick up. |
| `PULL_DOWN` | Pull the joystick down. |
| `PULL_LEFT` | Pull the joystick left. |
| `PULL_RIGHT` | Pull the joystick right. |
| `PRESS_FIRE` | Press the joystick button. |
| `PRESS_LEFT` | Press the left mouse button. |
| `PRESS_RIGHT` | Press the right mouse button. |
| `RELEASE_X` | Move back to neutral horizontally. |
| `RELEASE_Y` | Move back to neutral vertically. |
| `RELEASE_XY` | Move back to neutral. |
| `RELEASE_FIRE` | Release the joystick button. |
| `RELEASE_LEFT` | Release the left mouse button. |
| `RELEASE_RIGHT` | Release the right mouse button. |

---

### MouseModel

```cpp
enum MouseModel
```

Defined in Peripherals/Mouse/MouseTypes.h:25

Mouse model.

| Value | Description |
|-------|-------------|
| `C1350` | Joystick mouse (Commodore). |
| `C1351` | Analog mouse (Commodore). |
| `NEOS` | Analog mouse (Neos). |
| `PADDLE_X` | Paddle (POTX). |
| `PADDLE_Y` | Paddle (POTY). |
| `PADDLE_XY` | Paddle (POTX + POTX). |

---

### PaddleOrientation

```cpp
enum PaddleOrientation
```

Defined in Peripherals/Paddle/PaddleTypes.h:25

Value mapping scheme.

| Value | Description |
|-------|-------------|
| `HORIZONTAL` | Horizontal. |
| `HORIZONTAL_FLIPPED` | Horizontal, reverse direction. |
| `VERTICAL` | Vertical. |
| `VERTICAL_FLIPPED` | Vertical, reverse direction. |
