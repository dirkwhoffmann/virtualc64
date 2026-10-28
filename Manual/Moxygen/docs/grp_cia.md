---
title: "CIA"
---
# CIA

The two CIA chips and their time-of-day clocks.

## Classes

| Name | Description |
|------|-------------|
| [`CIAAPI`](vc64-CIAAPI.md#ciaapi) | CIA [API](vc64-API.md#api) |
| [`CIAConfig`](vc64-CIAConfig.md#ciaconfig) | Configuration of a CIA chip. |
| [`CIAInfo`](vc64-CIAInfo.md#ciainfo) | State of a CIA chip. |
| [`CIAMetrics`](vc64-CIAMetrics.md#ciametrics) | Usage statistics of a CIA chip. |
| [`CIAPortInfo`](vc64-CIAPortInfo.md#ciaportinfo) | State of a CIA port. |
| [`CIATimerInfo`](vc64-CIATimerInfo.md#ciatimerinfo) | State of a CIA timer. |
| [`TODInfo`](vc64-TODInfo.md#todinfo) | State of the time-of-day clock. |
| [`TimeOfDay`](vc64-TimeOfDay.md#timeofday) | Time-of-day value in BCD format. |

## Enumerations

| Name | Description |
|------|-------------|
| [`CIAReg`](#ciareg)  | Registers of a CIA chip. |
| [`CIARev`](#ciarev)  | Chip revision. |

---

### CIAReg

```cpp
enum CIAReg
```

Defined in Components/CIA/CIATypes.h:51

Registers of a CIA chip.

| Value | Description |
|-------|-------------|
| `PRA` |  |
| `PRB` |  |
| `DDRA` |  |
| `DDRB` |  |
| `TALO` |  |
| `TAHI` |  |
| `TBLO` |  |
| `TBHI` |  |
| `TODTHS` |  |
| `TODSEC` |  |
| `TODMIN` |  |
| `TODHR` |  |
| `SDR` |  |
| `ICR` |  |
| `CRA` |  |
| `CRB` |  |

---

### CIARev

```cpp
enum CIARev
```

Defined in Components/CIA/CIATypes.h:23

Chip revision.

| Value | Description |
|-------|-------------|
| `MOS_6526` |  |
| `MOS_8521` |  |
