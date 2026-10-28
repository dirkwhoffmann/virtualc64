---
title: "MouseAPI"
kind: class
---
# MouseAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class MouseAPI
```

Defined in VirtualC64.h:930

Mouse Public [API](/reference/vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`MouseConfig`](/reference/vc64-MouseConfig.md#mouseconfig) & | [`getConfig`](#getconfig-7) `const` | Returns the component's current configuration. |
| `void` | [`setXY`](#setxy)  | Moves the mouse |
| `void` | [`setDxDy`](#setdxdy)  | Moves the mouse |
| `void` | [`trigger`](#trigger-3)  | Triggers a mouse button event |
| `bool` | [`detectShakeXY`](#detectshakexy)  | Feeds a coordinate into the shake detector. |
| `bool` | [`detectShakeDxDy`](#detectshakedxdy)  | Feeds a coordinate into the shake detector. |

---

### getConfig

`const`

```cpp
const MouseConfig & getConfig() const
```

Defined in VirtualC64.h:940

Returns the component's current configuration.

---

### setXY

```cpp
void setXY(double x, double y)
```

Defined in VirtualC64.h:946

Moves the mouse

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `double` | New absolute horizontal coordinate |
| `y` | `double` | New absolute vertical coordinate |

---

### setDxDy

```cpp
void setDxDy(double dx, double dy)
```

Defined in VirtualC64.h:952

Moves the mouse

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `dx` | `double` | Relative horizontal mouse movement |
| `dy` | `double` | Relative vertical mouse movement |

---

### trigger

```cpp
void trigger(GamePadAction action)
```

Defined in VirtualC64.h:957

Triggers a mouse button event

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `action` | [`GamePadAction`](/reference/enum-GamePadAction.md) | The triggered event |

---

### detectShakeXY

```cpp
bool detectShakeXY(double x, double y)
```

Defined in VirtualC64.h:968

Feeds a coordinate into the shake detector.

The shake detector keeps track of the transmitted coordinates and scans for rapid movements caused by shaking the mouse.

#### Returns
true iff a shaking mouse has been detected.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `double` | Current horizontal mouse position. |
| `y` | `double` | Current vertical mouse position. |

---

### detectShakeDxDy

```cpp
bool detectShakeDxDy(double dx, double dy)
```

Defined in VirtualC64.h:981

Feeds a coordinate into the shake detector.

The shake detector keeps track of the transmitted coordinates and scans for rapid movements caused by shaking the mouse.

#### Returns
true iff a shaking mouse has been detected.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `dx` | `double` | Current horizontal mouse position, relative to the previous position. |
| `dy` | `double` | Current vertical mouse position, relative to the previous position. |

