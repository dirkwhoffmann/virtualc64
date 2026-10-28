---
title: "GuardsAPI"
---
# GuardsAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class GuardsAPI
```

Defined in VirtualC64.h:240

Guards [API](vc64-API.md#api)

Gives access to one list of guards (the breakpoints or the watchpoints of the CPU). The functions that modify the list come in two flavors, following the usual scheme of the [API](vc64-API.md#api):

* The plain functions (e.g., [setAt()](#setat)) return when the emulator thread has carried out the request. A failure throws the corresponding exception (e.g., if a guard to remove does not exist). They throw [CoreError](vc64-CoreError.md#coreerror) ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended, or if they are called from the emulator thread itself.
* The functions with the Async suffix (e.g., [setAtAsync()](#setatasync)) queue the request and return at once. A failure is not reported to the caller. The change shows up in the query functions only after the emulator thread has carried it out (see sync()).

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`elements`](#elements) `const` | Returns the number of guards in the guard list. |
| std::optional< [`GuardInfo`](vc64-GuardInfo.md#guardinfo) > | [`guardNr`](#guardnr) `const` | Returns information about a guard. |
| std::optional< [`GuardInfo`](vc64-GuardInfo.md#guardinfo) > | [`guardAt`](#guardat) `const` | Returns information about a guard. |
| `void` | [`setAt`](#setat)  | Sets a guard. |
| `void` | [`setAtAsync`](#setatasync)  | Asynchronous version of [setAt()](#setat): queues the request and returns at once. |
| `void` | [`moveTo`](#moveto-1)  | Relocates a guard. |
| `void` | [`moveToAsync`](#movetoasync)  | Asynchronous version of [moveTo()](#moveto-1): queues the request and returns at once. |
| `void` | [`remove`](#remove-4)  | Deletes a guard. |
| `void` | [`removeAsync`](#removeasync)  | Asynchronous version of [remove()](#remove-4): queues the request and returns at once. |
| `void` | [`removeAt`](#removeat)  | Deletes a guard. |
| `void` | [`removeAtAsync`](#removeatasync)  | Asynchronous version of [removeAt()](#removeat): queues the request and returns at once. |
| `void` | [`removeAll`](#removeall)  | Deletes all guards. |
| `void` | [`removeAllAsync`](#removeallasync)  | Asynchronous version of [removeAll()](#removeall): queues the request and returns at once. |
| `void` | [`enable`](#enable)  | Enables a guard. |
| `void` | [`enableAsync`](#enableasync)  | Asynchronous version of [enable()](#enable): queues the request and returns at once. |
| `void` | [`enableAt`](#enableat)  | Enables a guard. |
| `void` | [`enableAtAsync`](#enableatasync)  | Asynchronous version of [enableAt()](#enableat): queues the request and returns at once. |
| `void` | [`enableAll`](#enableall)  | Enables all guards. |
| `void` | [`enableAllAsync`](#enableallasync)  | Asynchronous version of [enableAll()](#enableall): queues the request and returns at once. |
| `void` | [`disable`](#disable)  | Disables a guard. |
| `void` | [`disableAsync`](#disableasync)  | Asynchronous version of [disable()](#disable): queues the request and returns at once. |
| `void` | [`disableAt`](#disableat)  | Disables a guard. |
| `void` | [`disableAtAsync`](#disableatasync)  | Asynchronous version of [disableAt()](#disableat): queues the request and returns at once. |
| `void` | [`disableAll`](#disableall)  | Disables all guards. |
| `void` | [`disableAllAsync`](#disableallasync)  | Asynchronous version of [disableAll()](#disableall): queues the request and returns at once. |
| `void` | [`toggle`](#toggle-1)  | Enables a disabled guard and disables an enabled one. |
| `void` | [`toggleAsync`](#toggleasync)  | Asynchronous version of [toggle()](#toggle-1): queues the request and returns at once. |

---

### elements

`const`

```cpp
isize elements() const
```

Defined in VirtualC64.h:253

Returns the number of guards in the guard list.

---

### guardNr

`const`

```cpp
std::optional< GuardInfo > guardNr(isize nr) const
```

Defined in VirtualC64.h:259

Returns information about a guard.

#### Returns
A copy of the guard's state, or nothing if it does not exist.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | Number of the guard in the guard list |

---

### guardAt

`const`

```cpp
std::optional< GuardInfo > guardAt(u32 target) const
```

Defined in VirtualC64.h:265

Returns information about a guard.

#### Returns
A copy of the guard's state, or nothing if the target is not guarded.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `u32` | The target of the guard to query |

---

### setAt

```cpp
void setAt(u32 target, isize ignores = 0)
```

Defined in VirtualC64.h:271

Sets a guard.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `u32` | The observed memory address (the target of the guard). |
| `ignores` | `isize` | If a value greater zero is given, the guard has to be hit the specified number of times until program execution is paused. |

---

### setAtAsync

```cpp
void setAtAsync(u32 target, isize ignores = 0)
```

Defined in VirtualC64.h:274

Asynchronous version of [setAt()](#setat): queues the request and returns at once.

---

### moveTo

```cpp
void moveTo(isize nr, u32 target)
```

Defined in VirtualC64.h:280

Relocates a guard.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | Number of the guard in the guard list |
| `target` | `u32` | New target |

---

### moveToAsync

```cpp
void moveToAsync(isize nr, u32 target)
```

Defined in VirtualC64.h:283

Asynchronous version of [moveTo()](#moveto-1): queues the request and returns at once.

---

### remove

```cpp
void remove(isize nr)
```

Defined in VirtualC64.h:288

Deletes a guard.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | Number of the guard in the guard list |

---

### removeAsync

```cpp
void removeAsync(isize nr)
```

Defined in VirtualC64.h:291

Asynchronous version of [remove()](#remove-4): queues the request and returns at once.

---

### removeAt

```cpp
void removeAt(u32 target)
```

Defined in VirtualC64.h:296

Deletes a guard.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `u32` | The target of the guard to be deleted. |

---

### removeAtAsync

```cpp
void removeAtAsync(u32 target)
```

Defined in VirtualC64.h:299

Asynchronous version of [removeAt()](#removeat): queues the request and returns at once.

---

### removeAll

```cpp
void removeAll()
```

Defined in VirtualC64.h:303

Deletes all guards.

---

### removeAllAsync

```cpp
void removeAllAsync()
```

Defined in VirtualC64.h:306

Asynchronous version of [removeAll()](#removeall): queues the request and returns at once.

---

### enable

```cpp
void enable(isize nr)
```

Defined in VirtualC64.h:311

Enables a guard.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | Number of the guard in the guard list |

---

### enableAsync

```cpp
void enableAsync(isize nr)
```

Defined in VirtualC64.h:314

Asynchronous version of [enable()](#enable): queues the request and returns at once.

---

### enableAt

```cpp
void enableAt(u32 target)
```

Defined in VirtualC64.h:319

Enables a guard.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `u32` | The target of the guard to be enabled |

---

### enableAtAsync

```cpp
void enableAtAsync(u32 target)
```

Defined in VirtualC64.h:322

Asynchronous version of [enableAt()](#enableat): queues the request and returns at once.

---

### enableAll

```cpp
void enableAll()
```

Defined in VirtualC64.h:326

Enables all guards.

---

### enableAllAsync

```cpp
void enableAllAsync()
```

Defined in VirtualC64.h:329

Asynchronous version of [enableAll()](#enableall): queues the request and returns at once.

---

### disable

```cpp
void disable(isize nr)
```

Defined in VirtualC64.h:334

Disables a guard.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | Number of the guard in the guard list |

---

### disableAsync

```cpp
void disableAsync(isize nr)
```

Defined in VirtualC64.h:337

Asynchronous version of [disable()](#disable): queues the request and returns at once.

---

### disableAt

```cpp
void disableAt(u32 target)
```

Defined in VirtualC64.h:342

Disables a guard.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `u32` | The target of the guard to be disabled |

---

### disableAtAsync

```cpp
void disableAtAsync(u32 target)
```

Defined in VirtualC64.h:345

Asynchronous version of [disableAt()](#disableat): queues the request and returns at once.

---

### disableAll

```cpp
void disableAll()
```

Defined in VirtualC64.h:349

Disables all guards.

---

### disableAllAsync

```cpp
void disableAllAsync()
```

Defined in VirtualC64.h:352

Asynchronous version of [disableAll()](#disableall): queues the request and returns at once.

---

### toggle

```cpp
void toggle(isize nr)
```

Defined in VirtualC64.h:357

Enables a disabled guard and disables an enabled one.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | Number of the guard in the guard list |

---

### toggleAsync

```cpp
void toggleAsync(isize nr)
```

Defined in VirtualC64.h:360

Asynchronous version of [toggle()](#toggle-1): queues the request and returns at once.

