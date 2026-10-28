---
title: "SIDConfig"
kind: struct
---
# SIDConfig

```cpp
#include <Components/SID/SIDTypes.h>
```

```cpp
struct SIDConfig
```

Defined in Components/SID/SIDTypes.h:138

Configuration of a SID chip.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`enabled`](#enabled-2)  | Indicates if this SID is plugged in. |
| `u16` | [`address`](#address)  | Location of the SID registers in memory. |
| [`SIDRevision`](enum-SIDRevision.md) | [`revision`](#revision-2)  | Chip revision (MOS 6581 or MOS 8580). |
| `bool` | [`filter`](#filter)  | Use filter. |
| [`SIDEngine`](enum-SIDEngine.md) | [`engine`](#engine)  | Emulation engine. |
| [`SamplingMethod`](enum-SamplingMethod.md) | [`sampling`](#sampling)  | Sampling method. |
| `bool` | [`powerSave`](#powersave-1)  | Performance. |

---

### enabled

```cpp
bool enabled
```

Defined in Components/SID/SIDTypes.h:141

Indicates if this SID is plugged in.

---

### address

```cpp
u16 address
```

Defined in Components/SID/SIDTypes.h:144

Location of the SID registers in memory.

---

### revision

```cpp
SIDRevision revision
```

Type: [`SIDRevision`](enum-SIDRevision.md)

Defined in Components/SID/SIDTypes.h:147

Chip revision (MOS 6581 or MOS 8580).

---

### filter

```cpp
bool filter
```

Defined in Components/SID/SIDTypes.h:148

Use filter.

---

### engine

```cpp
SIDEngine engine
```

Type: [`SIDEngine`](enum-SIDEngine.md)

Defined in Components/SID/SIDTypes.h:151

Emulation engine.

---

### sampling

```cpp
SamplingMethod sampling
```

Type: [`SamplingMethod`](enum-SamplingMethod.md)

Defined in Components/SID/SIDTypes.h:152

Sampling method.

---

### powerSave

```cpp
bool powerSave
```

Defined in Components/SID/SIDTypes.h:155

Performance.

