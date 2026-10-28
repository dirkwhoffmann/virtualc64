---
title: "CPUInfo"
---
# CPUInfo

```cpp
#include <Components/CPU/CPUTypes.h>
```

```cpp
struct CPUInfo
```

Defined in Components/CPU/CPUTypes.h:133

State of the CPU.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `i64` | [`cycle`](#cycle)  | Cycle counter. |
| `u16` | [`pc0`](#pc0)  | Address of the current instruction. |
| `u16` | [`pc`](#pc)  | Program counter. |
| `u8` | [`sp`](#sp)  | Stack pointer. |
| `u8` | [`a`](#a)  | Accumulator. |
| `u8` | [`x`](#x)  | X register. |
| `u8` | [`y`](#y)  | Y register. |
| `u8` | [`sr`](#sr)  | Status register. |
| `u8` | [`irq`](#irq)  | State of the IRQ lines. |
| `u8` | [`nmi`](#nmi)  | State of the NMI lines. |
| `bool` | [`rdy`](#rdy)  | State of the RDY line. |
| `bool` | [`jammed`](#jammed)  | The CPU is jammed. |
| `long` | [`next`](#next)  | Next micro instruction to be executed. |
| `u8` | [`processorPort`](#processorport)  | Processor port register. |
| `u8` | [`processorPortDir`](#processorportdir)  | Processor port data direction register. |

---

### cycle

```cpp
i64 cycle
```

Defined in Components/CPU/CPUTypes.h:135

Cycle counter.

---

### pc0

```cpp
u16 pc0
```

Defined in Components/CPU/CPUTypes.h:137

Address of the current instruction.

---

### pc

```cpp
u16 pc
```

Defined in Components/CPU/CPUTypes.h:138

Program counter.

---

### sp

```cpp
u8 sp
```

Defined in Components/CPU/CPUTypes.h:139

Stack pointer.

---

### a

```cpp
u8 a
```

Defined in Components/CPU/CPUTypes.h:140

Accumulator.

---

### x

```cpp
u8 x
```

Defined in Components/CPU/CPUTypes.h:141

X register.

---

### y

```cpp
u8 y
```

Defined in Components/CPU/CPUTypes.h:142

Y register.

---

### sr

```cpp
u8 sr
```

Defined in Components/CPU/CPUTypes.h:143

Status register.

---

### irq

```cpp
u8 irq
```

Defined in Components/CPU/CPUTypes.h:145

State of the IRQ lines.

---

### nmi

```cpp
u8 nmi
```

Defined in Components/CPU/CPUTypes.h:146

State of the NMI lines.

---

### rdy

```cpp
bool rdy
```

Defined in Components/CPU/CPUTypes.h:147

State of the RDY line.

---

### jammed

```cpp
bool jammed
```

Defined in Components/CPU/CPUTypes.h:148

The CPU is jammed.

---

### next

```cpp
long next
```

Defined in Components/CPU/CPUTypes.h:150

Next micro instruction to be executed.

---

### processorPort

```cpp
u8 processorPort
```

Defined in Components/CPU/CPUTypes.h:152

Processor port register.

---

### processorPortDir

```cpp
u8 processorPortDir
```

Defined in Components/CPU/CPUTypes.h:153

Processor port data direction register.

