---
title: "CIAMetrics"
kind: struct
---
# CIAMetrics

```cpp
#include <Components/CIA/CIATypes.h>
```

```cpp
struct CIAMetrics
```

Defined in Components/CIA/CIATypes.h:162

Usage statistics of a CIA chip.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`Cycle`](vc64.md#cycle) | [`totalCycles`](#totalcycles)  | Total number of executed cycles. |
| [`Cycle`](vc64.md#cycle) | [`idleCycles`](#idlecycles)  | Number of cycles spent in the idle state. |
| [`Cycle`](vc64.md#cycle) | [`idleSince`](#idlesince)  | [Cycle](vc64.md#cycle) at which the idle state started. |
| `double` | [`idlePercentage`](#idlepercentage)  | Share of idle cycles. |

---

### totalCycles

```cpp
Cycle totalCycles
```

Type: [`Cycle`](vc64.md#cycle)

Defined in Components/CIA/CIATypes.h:164

Total number of executed cycles.

---

### idleCycles

```cpp
Cycle idleCycles
```

Type: [`Cycle`](vc64.md#cycle)

Defined in Components/CIA/CIATypes.h:165

Number of cycles spent in the idle state.

---

### idleSince

```cpp
Cycle idleSince
```

Type: [`Cycle`](vc64.md#cycle)

Defined in Components/CIA/CIATypes.h:166

[Cycle](vc64.md#cycle) at which the idle state started.

---

### idlePercentage

```cpp
double idlePercentage
```

Defined in Components/CIA/CIATypes.h:168

Share of idle cycles.

