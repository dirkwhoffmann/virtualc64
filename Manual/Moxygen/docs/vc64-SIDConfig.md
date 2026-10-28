---
title: "SIDConfig"
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
| [`SIDRevision`](vc64.md#sidrevision) | [`revision`](#revision-2)  | Chip revision (MOS 6581 or MOS 8580). |
| `bool` | [`filter`](#filter)  | Use filter. |
| [`SIDEngine`](vc64.md#sidengine) | [`engine`](#engine)  | Emulation engine. |
| [`SamplingMethod`](vc64.md#samplingmethod) | [`sampling`](#sampling)  | Sampling method. |
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

Type: [`SIDRevision`](vc64.md#sidrevision)

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

Type: [`SIDEngine`](vc64.md#sidengine)

Defined in Components/SID/SIDTypes.h:151

Emulation engine.

---

### sampling

```cpp
SamplingMethod sampling
```

Type: [`SamplingMethod`](vc64.md#samplingmethod)

Defined in Components/SID/SIDTypes.h:152

Sampling method.

---

### powerSave

```cpp
bool powerSave
```

Defined in Components/SID/SIDTypes.h:155

Performance.

