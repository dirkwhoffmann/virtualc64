---
title: "KeyboardAPI"
kind: class
---
# KeyboardAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class KeyboardAPI
```

Defined in VirtualC64.h:827

Keyboard Public [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`isPressed`](#ispressed) `const` | Checks if a key is currently pressed. |
| `void` | [`press`](#press)  | Presses a key. |
| `void` | [`toggle`](#toggle-2)  | Toggles a key. |
| `void` | [`release`](#release)  | Releases a key. |
| `void` | [`releaseAll`](#releaseall)  | Releases all currently pressed keys. |
| `bool` | [`isLocked`](#islocked) `const` | Checks if a key is currently pressed. |
| `void` | [`lock`](#lock)  | Locks a key. |
| `void` | [`unlock`](#unlock)  | Unlocks a key. |
| `void` | [`unlockAll`](#unlockall)  | Unlocks all keys. |
| `void` | [`autoType`](#autotype)  | Uses the auto-typing daemon to type a string. |
| `void` | [`abortTyping`](#aborttyping)  | Aborts any active auto-typing activity. |

---

### isPressed

`const`

```cpp
bool isPressed(C64Key key) const
```

Defined in VirtualC64.h:838

Checks if a key is currently pressed.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | [`C64Key`](vc64-C64Key.md#c64key) | The key to check. |

---

### press

```cpp
void press(C64Key key, double delay = 0.0, double duration = 0.0)
```

Defined in VirtualC64.h:855

Presses a key.

If no delay is specified, the function will immediately modify the C64's keyboard matrix. Otherwise, it will ask the event scheduler to modify the matrix with the specified delay.

> [!NOTE]
> If you wish to press multiple keys, make sure to let some time pass between two key presses. You need to give the C64 time to scan the keyboard matrix before another key can be pressed.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | [`C64Key`](vc64-C64Key.md#c64key) | The key to press. |
| `delay` | `double` | An optional delay in seconds. |
| `duration` | `double` | An optional time in seconds after which the key is released again. If 0, the key stays pressed until it is released explicitly. |

---

### toggle

```cpp
void toggle(C64Key key, double delay = 0.0, double duration = 0.0)
```

Defined in VirtualC64.h:867

Toggles a key.

If no delay is specified, the function will immediately modify the C64's keyboard matrix. Otherwise, it will ask the event scheduler to modify the matrix with the specified delay.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | [`C64Key`](vc64-C64Key.md#c64key) | The key to press or release. |
| `delay` | `double` | An optional delay in seconds. |
| `duration` | `double` | An optional time in seconds after which the key is toggled back. If 0, the key is toggled only once. |

---

### release

```cpp
void release(C64Key key, double delay = 0.0)
```

Defined in VirtualC64.h:877

Releases a key.

If no delay is specified, the function will immediately modify the C64's keyboard matrix. Otherwise, it will ask the event scheduler to modify the matrix with the specified delay.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | [`C64Key`](vc64-C64Key.md#c64key) | The key to release. |
| `delay` | `double` | An optional delay in seconds. |

---

### releaseAll

```cpp
void releaseAll(double delay = 0.0)
```

Defined in VirtualC64.h:881

Releases all currently pressed keys.

---

### isLocked

`const`

```cpp
bool isLocked(C64Key key) const
```

Defined in VirtualC64.h:886

Checks if a key is currently pressed.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | [`C64Key`](vc64-C64Key.md#c64key) | The key to check. |

---

### lock

```cpp
void lock(C64Key key, double delay = 0.0)
```

Defined in VirtualC64.h:897

Locks a key.

When a key is marked as locked, it won't be released when [release()](#release) or [releaseAll()](#releaseall) is called.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | [`C64Key`](vc64-C64Key.md#c64key) | The key to lock. |
| `delay` | `double` | An optional delay in seconds. |

---

### unlock

```cpp
void unlock(C64Key key, double delay = 0.0)
```

Defined in VirtualC64.h:907

Unlocks a key.

Removes the lock, putting the key back into it's normal state.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | [`C64Key`](vc64-C64Key.md#c64key) | The key to unlock. |
| `delay` | `double` | An optional delay in seconds. |

---

### unlockAll

```cpp
void unlockAll(double delay = 0.0)
```

Defined in VirtualC64.h:915

Unlocks all keys.

Removes the lock for all keys, putting them back into their normal state.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `delay` | `double` | An optional delay in seconds. |

---

### autoType

```cpp
void autoType(const string & text)
```

Defined in VirtualC64.h:920

Uses the auto-typing daemon to type a string.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `const string &` | The text to type. |

---

### abortTyping

```cpp
void abortTyping()
```

Defined in VirtualC64.h:924

Aborts any active auto-typing activity.

