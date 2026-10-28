---
title: "CoreError"
---
# CoreError

```cpp
#include <Infrastructure/CoreError.h>
```

```cpp
struct CoreError
```

Defined in Infrastructure/CoreError.h:26

Exception thrown if a request does not match the state of the emulator.

A [CoreError](#coreerror) is thrown, for example, if a configuration option is set to an invalid value, if a ROM is missing, or if the emulator thread cannot carry out a request. The error code is stored in the payload and is one of the constants below. It can be queried with the inherited member functions.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `const char *` | [`errstr`](#errstr-2) `virtual` `const` `inline` `noexcept` `override` | Returns the name of the error code. |
|  | [`CoreError`](#coreerror-1) `explicit` | Creates an error with an error code and a descriptive string. |
|  | [`CoreError`](#coreerror-2) `inline` `explicit` | Creates an error with an error code and a descriptive string. |
|  | [`CoreError`](#coreerror-3) `inline` `explicit` | Creates an error with an error code and a file path. |
|  | [`CoreError`](#coreerror-4) `inline` `explicit` | Creates an error with an error code and a number. |
|  | [`CoreError`](#coreerror-5) `inline` `explicit` | Creates an error with an error code. |

---

### errstr

`virtual` `const` `inline` `noexcept` `override`

```cpp
virtual inline const char * errstr() const noexcept override
```

Defined in Infrastructure/CoreError.h:72

Returns the name of the error code.

#### Reimplements

- [`Error::errstr`](utl-Error.md#errstr)

---

### CoreError

`explicit`

```cpp
explicit CoreError(long fault, const string & s)
```

Defined in Infrastructure/CoreError.h:116

Creates an error with an error code and a descriptive string.

---

### CoreError

`inline` `explicit`

```cpp
inline explicit CoreError(long fault, const char * s)
```

Defined in Infrastructure/CoreError.h:118

Creates an error with an error code and a descriptive string.

---

### CoreError

`inline` `explicit`

```cpp
inline explicit CoreError(long fault, const fs::path & p)
```

Defined in Infrastructure/CoreError.h:120

Creates an error with an error code and a file path.

---

### CoreError

`inline` `explicit`

```cpp
inline explicit CoreError(long fault, std::integral auto v)
```

Defined in Infrastructure/CoreError.h:122

Creates an error with an error code and a number.

---

### CoreError

`inline` `explicit`

```cpp
inline explicit CoreError(long fault)
```

Defined in Infrastructure/CoreError.h:124

Creates an error with an error code.

## Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `long` | [`OK`](#ok) `static` `constexpr` | No error. |
| `long` | [`UNKNOWN`](#unknown) `static` `constexpr` | Unclassified error condition. |
| `long` | [`LAUNCH_FAILED`](#launch_failed) `static` `constexpr` | Launching the emulator thread failed. |
| `long` | [`NOT_LAUNCHED`](#not_launched) `static` `constexpr` | The emulator thread is not running. |
| `long` | [`ALREADY_LAUNCHED`](#already_launched) `static` `constexpr` | The emulator thread has been launched already. |
| `long` | [`POWERED_OFF`](#powered_off) `static` `constexpr` | The emulator is powered off. |
| `long` | [`POWERED_ON`](#powered_on) `static` `constexpr` | The emulator is powered on. |
| `long` | [`RUNNING`](#running-1) `static` `constexpr` | The emulator is running. |
| `long` | [`OPT_UNSUPPORTED`](#opt_unsupported) `static` `constexpr` | Unsupported configuration option. |
| `long` | [`OPT_INV_ARG`](#opt_inv_arg) `static` `constexpr` | Invalid argument. |
| `long` | [`OPT_INV_ID`](#opt_inv_id) `static` `constexpr` | Invalid component id. |
| `long` | [`OPT_LOCKED`](#opt_locked) `static` `constexpr` | The option is temporarily locked. |
| `long` | [`INVALID_KEY`](#invalid_key) `static` `constexpr` | Invalid property key. |
| `long` | [`SYNTAX`](#syntax) `static` `constexpr` | Syntax error. |
| `long` | [`GUARD_NOT_FOUND`](#guard_not_found) `static` `constexpr` | [Guard](vc64-Guard.md#guard) is not set. |
| `long` | [`GUARD_ALREADY_SET`](#guard_already_set) `static` `constexpr` | [Guard](vc64-Guard.md#guard) is already set. |
| `long` | [`BP_NOT_FOUND`](#bp_not_found) `static` `constexpr` | Breakpoint is not set (DEPRECATED). |
| `long` | [`BP_ALREADY_SET`](#bp_already_set) `static` `constexpr` | Breakpoint is already set (DEPRECATED). |
| `long` | [`WP_NOT_FOUND`](#wp_not_found) `static` `constexpr` | Watchpoint is not set (DEPRECATED). |
| `long` | [`WP_ALREADY_SET`](#wp_already_set) `static` `constexpr` | Watchpoint is already set (DEPRECATED). |
| `long` | [`OUT_OF_MEMORY`](#out_of_memory) `static` `constexpr` | Out of memory. |
| `long` | [`ROM_BASIC_MISSING`](#rom_basic_missing) `static` `constexpr` | No Basic Rom installed. |
| `long` | [`ROM_CHAR_MISSING`](#rom_char_missing) `static` `constexpr` | No Character Rom installed. |
| `long` | [`ROM_KERNAL_MISSING`](#rom_kernal_missing) `static` `constexpr` | No Kernal Rom installed. |
| `long` | [`ROM_DRIVE_MISSING`](#rom_drive_missing) `static` `constexpr` | No Floppy Drive Rom installed. |
| `long` | [`ROM_MEGA65_MISMATCH`](#rom_mega65_mismatch) `static` `constexpr` | MEGA65 Roms have different version numbers. |
| `long` | [`DRV_UNCONNECTED`](#drv_unconnected) `static` `constexpr` | Floppy drive is not connected. |
| `long` | [`DRV_NO_DISK`](#drv_no_disk) `static` `constexpr` | Floppy drive contains no disk. |

---

### OK

`static` `constexpr`

```cpp
long OK =   0
```

Defined in Infrastructure/CoreError.h:28

No error.

---

### UNKNOWN

`static` `constexpr`

```cpp
long UNKNOWN =   1
```

Defined in Infrastructure/CoreError.h:29

Unclassified error condition.

---

### LAUNCH_FAILED

`static` `constexpr`

```cpp
long LAUNCH_FAILED = 100
```

Defined in Infrastructure/CoreError.h:32

Launching the emulator thread failed.

---

### NOT_LAUNCHED

`static` `constexpr`

```cpp
long NOT_LAUNCHED = 101
```

Defined in Infrastructure/CoreError.h:33

The emulator thread is not running.

---

### ALREADY_LAUNCHED

`static` `constexpr`

```cpp
long ALREADY_LAUNCHED = 102
```

Defined in Infrastructure/CoreError.h:34

The emulator thread has been launched already.

---

### POWERED_OFF

`static` `constexpr`

```cpp
long POWERED_OFF = 103
```

Defined in Infrastructure/CoreError.h:35

The emulator is powered off.

---

### POWERED_ON

`static` `constexpr`

```cpp
long POWERED_ON = 104
```

Defined in Infrastructure/CoreError.h:36

The emulator is powered on.

---

### RUNNING

`static` `constexpr`

```cpp
long RUNNING = 105
```

Defined in Infrastructure/CoreError.h:37

The emulator is running.

---

### OPT_UNSUPPORTED

`static` `constexpr`

```cpp
long OPT_UNSUPPORTED = 200
```

Defined in Infrastructure/CoreError.h:40

Unsupported configuration option.

---

### OPT_INV_ARG

`static` `constexpr`

```cpp
long OPT_INV_ARG = 201
```

Defined in Infrastructure/CoreError.h:41

Invalid argument.

---

### OPT_INV_ID

`static` `constexpr`

```cpp
long OPT_INV_ID = 202
```

Defined in Infrastructure/CoreError.h:42

Invalid component id.

---

### OPT_LOCKED

`static` `constexpr`

```cpp
long OPT_LOCKED = 203
```

Defined in Infrastructure/CoreError.h:43

The option is temporarily locked.

---

### INVALID_KEY

`static` `constexpr`

```cpp
long INVALID_KEY = 300
```

Defined in Infrastructure/CoreError.h:46

Invalid property key.

---

### SYNTAX

`static` `constexpr`

```cpp
long SYNTAX = 301
```

Defined in Infrastructure/CoreError.h:47

Syntax error.

---

### GUARD_NOT_FOUND

`static` `constexpr`

```cpp
long GUARD_NOT_FOUND = 401
```

Defined in Infrastructure/CoreError.h:50

[Guard](vc64-Guard.md#guard) is not set.

---

### GUARD_ALREADY_SET

`static` `constexpr`

```cpp
long GUARD_ALREADY_SET = 402
```

Defined in Infrastructure/CoreError.h:51

[Guard](vc64-Guard.md#guard) is already set.

---

### BP_NOT_FOUND

`static` `constexpr`

```cpp
long BP_NOT_FOUND = 403
```

Defined in Infrastructure/CoreError.h:52

Breakpoint is not set (DEPRECATED).

---

### BP_ALREADY_SET

`static` `constexpr`

```cpp
long BP_ALREADY_SET = 404
```

Defined in Infrastructure/CoreError.h:53

Breakpoint is already set (DEPRECATED).

---

### WP_NOT_FOUND

`static` `constexpr`

```cpp
long WP_NOT_FOUND = 405
```

Defined in Infrastructure/CoreError.h:54

Watchpoint is not set (DEPRECATED).

---

### WP_ALREADY_SET

`static` `constexpr`

```cpp
long WP_ALREADY_SET = 406
```

Defined in Infrastructure/CoreError.h:55

Watchpoint is already set (DEPRECATED).

---

### OUT_OF_MEMORY

`static` `constexpr`

```cpp
long OUT_OF_MEMORY = 500
```

Defined in Infrastructure/CoreError.h:58

Out of memory.

---

### ROM_BASIC_MISSING

`static` `constexpr`

```cpp
long ROM_BASIC_MISSING = 600
```

Defined in Infrastructure/CoreError.h:61

No Basic Rom installed.

---

### ROM_CHAR_MISSING

`static` `constexpr`

```cpp
long ROM_CHAR_MISSING = 601
```

Defined in Infrastructure/CoreError.h:62

No Character Rom installed.

---

### ROM_KERNAL_MISSING

`static` `constexpr`

```cpp
long ROM_KERNAL_MISSING = 602
```

Defined in Infrastructure/CoreError.h:63

No Kernal Rom installed.

---

### ROM_DRIVE_MISSING

`static` `constexpr`

```cpp
long ROM_DRIVE_MISSING = 603
```

Defined in Infrastructure/CoreError.h:64

No Floppy Drive Rom installed.

---

### ROM_MEGA65_MISMATCH

`static` `constexpr`

```cpp
long ROM_MEGA65_MISMATCH = 604
```

Defined in Infrastructure/CoreError.h:65

MEGA65 Roms have different version numbers.

---

### DRV_UNCONNECTED

`static` `constexpr`

```cpp
long DRV_UNCONNECTED = 700
```

Defined in Infrastructure/CoreError.h:68

Floppy drive is not connected.

---

### DRV_NO_DISK

`static` `constexpr`

```cpp
long DRV_NO_DISK = 701
```

Defined in Infrastructure/CoreError.h:69

Floppy drive contains no disk.

