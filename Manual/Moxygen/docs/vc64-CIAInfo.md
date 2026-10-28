---
title: "CIAInfo"
kind: struct
---
# CIAInfo

```cpp
#include <Components/CIA/CIATypes.h>
```

```cpp
struct CIAInfo
```

Defined in Components/CIA/CIATypes.h:142

State of a CIA chip.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`CIAPortInfo`](vc64-CIAPortInfo.md#ciaportinfo) | [`portA`](#porta)  | Port A. |
| [`CIAPortInfo`](vc64-CIAPortInfo.md#ciaportinfo) | [`portB`](#portb)  | Port B. |
| [`CIATimerInfo`](vc64-CIATimerInfo.md#ciatimerinfo) | [`timerA`](#timera)  | Timer A. |
| [`CIATimerInfo`](vc64-CIATimerInfo.md#ciatimerinfo) | [`timerB`](#timerb)  | Timer B. |
| `u8` | [`sdr`](#sdr)  | Serial data register (SDR). |
| `u8` | [`ssr`](#ssr)  | Serial shift register. |
| `u8` | [`icr`](#icr)  | Interrupt control register (pending interrupts). |
| `u8` | [`imr`](#imr)  | Interrupt mask register. |
| `bool` | [`intLine`](#intline)  | The interrupt line is active. |
| [`TODInfo`](vc64-TODInfo.md#todinfo) | [`tod`](#tod)  | Time-of-day clock. |
| `bool` | [`todIntEnable`](#todintenable)  | The TOD alarm interrupt is enabled. |

---

### portA

```cpp
CIAPortInfo portA
```

Type: [`CIAPortInfo`](vc64-CIAPortInfo.md#ciaportinfo)

Defined in Components/CIA/CIATypes.h:144

Port A.

---

### portB

```cpp
CIAPortInfo portB
```

Type: [`CIAPortInfo`](vc64-CIAPortInfo.md#ciaportinfo)

Defined in Components/CIA/CIATypes.h:145

Port B.

---

### timerA

```cpp
CIATimerInfo timerA
```

Type: [`CIATimerInfo`](vc64-CIATimerInfo.md#ciatimerinfo)

Defined in Components/CIA/CIATypes.h:147

Timer A.

---

### timerB

```cpp
CIATimerInfo timerB
```

Type: [`CIATimerInfo`](vc64-CIATimerInfo.md#ciatimerinfo)

Defined in Components/CIA/CIATypes.h:148

Timer B.

---

### sdr

```cpp
u8 sdr
```

Defined in Components/CIA/CIATypes.h:150

Serial data register (SDR).

---

### ssr

```cpp
u8 ssr
```

Defined in Components/CIA/CIATypes.h:151

Serial shift register.

---

### icr

```cpp
u8 icr
```

Defined in Components/CIA/CIATypes.h:152

Interrupt control register (pending interrupts).

---

### imr

```cpp
u8 imr
```

Defined in Components/CIA/CIATypes.h:153

Interrupt mask register.

---

### intLine

```cpp
bool intLine
```

Defined in Components/CIA/CIATypes.h:154

The interrupt line is active.

---

### tod

```cpp
TODInfo tod
```

Type: [`TODInfo`](vc64-TODInfo.md#todinfo)

Defined in Components/CIA/CIATypes.h:156

Time-of-day clock.

---

### todIntEnable

```cpp
bool todIntEnable
```

Defined in Components/CIA/CIATypes.h:157

The TOD alarm interrupt is enabled.

