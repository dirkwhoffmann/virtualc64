---
title: "JoystickAPI"
kind: class
---
# JoystickAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class JoystickAPI
```

Defined in VirtualC64.h:987

Joystick Public [API](/reference/vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`JoystickConfig`](/reference/vc64-JoystickConfig.md#joystickconfig) & | [`getConfig`](#getconfig-5) `const` | Returns the component's current configuration. |
| const [`JoystickInfo`](/reference/vc64-JoystickInfo.md#joystickinfo) & | [`getInfo`](#getinfo-7) `const` | Returns the component's current state. |
| const [`JoystickInfo`](/reference/vc64-JoystickInfo.md#joystickinfo) & | [`getCachedInfo`](#getcachedinfo-7) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-7)). |
| `void` | [`trigger`](#trigger-1)  | Triggers a joystick action. |
| `void` | [`trigger`](#trigger-2)  | Sets the complete joystick state. |

---

### getConfig

`const`

```cpp
const JoystickConfig & getConfig() const
```

Defined in VirtualC64.h:997

Returns the component's current configuration.

---

### getInfo

`const`

```cpp
const JoystickInfo & getInfo() const
```

Defined in VirtualC64.h:1008

Returns the component's current state.

[getInfo()](#getinfo-7) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-7) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const JoystickInfo & getCachedInfo() const
```

Defined in VirtualC64.h:1010

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-7)).

---

### trigger

```cpp
void trigger(GamePadAction event)
```

Defined in VirtualC64.h:1014

Triggers a joystick action.

---

### trigger

```cpp
void trigger(const std::array< bool, 5 > & state, const std::array< bool, 5 > & previous)
```

Defined in VirtualC64.h:1026

Sets the complete joystick state.

The states are ordered up, down, left, right, fire. The directions are applied as given, which is harmless to repeat. The fire button is only forwarded if it differs in 'previous', because pressing it again would toggle autofire. Pass the state that was reported by the previous call.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `const std::array< bool, 5 > &` | The new states of up, down, left, right, and fire. |
| `previous` | `const std::array< bool, 5 > &` | The previous states. |

